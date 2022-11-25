/*
*  tcp_scanner.cpp
*  ---------------
*  Source file for an IPv4 TCP network scanner
*/
#include <conio.h>
#include "includes/inet/http/request.h"
#include "includes/inet/scanners/tcp_scanner.h"
#include "includes/inet/sockets/tls_client.h"
#include "includes/resources/resource.h"
#include "includes/utils/arg_parser.h"

/**
* @brief  Initialize the object.
*/
scan::TcpScanner::TcpScanner(TcpScanner &&t_scanner) noexcept
    : m_ioc(t_scanner.m_ioc) {

    *this = std::forward<this_t>(t_scanner);
}

/**
* @brief  Initialize the object.
*/
scan::TcpScanner::TcpScanner(io_context &t_ioc, shared_ptr<Args> t_argsp)
    : m_ioc(t_ioc), m_pool(t_argsp->threads) {

    m_trc_ap = std::make_shared<TextRc>(CSV_DATA);
    parse_argsp(t_argsp);
}

/**
* @brief  Move assignment operator overload.
*/
scan::TcpScanner &scan::TcpScanner::operator=(TcpScanner &&t_scanner) noexcept
{
    if (this != &t_scanner)
    {
        std::scoped_lock lock{ m_ports_mtx, m_services_mtx, m_statuses_mtx };

        m_args_ap = t_scanner.m_args_ap.load();
        m_conn_timeout = t_scanner.m_conn_timeout;
        m_services = t_scanner.m_services;
        m_statuses = t_scanner.m_statuses;
        m_threads = t_scanner.m_threads;
        m_timer = t_scanner.m_timer;
        m_trc_ap = t_scanner.m_trc_ap.load();
        m_uri = t_scanner.m_uri;

        out_json = t_scanner.out_json.load();
        out_path = t_scanner.out_path;
        ports = t_scanner.ports;
        target = t_scanner.target;
        verbose = t_scanner.verbose.load();
    }
    return *this;
}

/**
* @brief  Set the scanner connection timeout duration.
*/
void scan::TcpScanner::connect_timeout(const Timeout &t_timeout)
{
    m_conn_timeout = t_timeout;
}

/**
* @brief  Perform the network service scan against the target.
*         Locks the underlying port list mutex.
*/
void scan::TcpScanner::scan()
{
    if (!target.is_valid())
    {
        throw RuntimeEx{ "TcpScanner::scan", "Invalid underlying target hostname" };
    }

    // Post scan tasks to the thread pool
    {
        std::scoped_lock lock{ m_ports_mtx };

        if (!net::valid_port(ports))
        {
            throw RuntimeEx{ "TcpScanner::scan", "Invalid underlying port(s)" };
        }

        scan_startup();

        for (const uint_t &port : ports)
        {
            post_port_scan(port);
        }
    }

    m_pool.wait();
    scan_shutdown();
}

/**
* @brief  Block execution until all outstanding scan tasks in the underlying
*         thread pool have completed executing.
*/
void scan::TcpScanner::wait()
{
    m_pool.wait();
}

/**
* @brief  Add service information to the underlying service list.
*         Locks the underlying service list mutex.
*/
void scan::TcpScanner::add_service(const SvcInfo &t_info)
{
    std::scoped_lock lock{ m_services_mtx };
    m_services.add(t_info);
}

/**
* @brief  Parse information from the given command-line arguments smart pointer.
*         Locks the underlying port list mutex.
*/
void scan::TcpScanner::parse_argsp(shared_ptr<Args> t_argsp)
{
    m_args_ap = t_argsp;
    m_conn_timeout = t_argsp->timeout;
    m_threads = t_argsp->threads;
    m_uri = t_argsp->uri;

    out_json = t_argsp->out_json;
    out_path = t_argsp->out_path;
    target = t_argsp->target;
    verbose = t_argsp->verbose;

    {
        std::scoped_lock lock{ m_ports_mtx };

        for (const uint_t &port : ports = t_argsp->ports)
        {
            set_status(port, TaskStatus::not_started);
        }
    }
}

/**
* @brief  Create a new port scan task and submit it to the underlying
*         thread pool for execution.
*/
void scan::TcpScanner::post_port_scan(const uint_t &t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number specified" };
    }

    if (!target.is_valid())
    {
        throw RuntimeEx{ "TcpScanner::post_port_scan", "Invalid underlying target" };
    }

    // Post a new scan task to the thread pool
    m_pool.post([&, this]() mutable -> void
    {
        print_progress();
        set_status(t_port, TaskStatus::executing);

        io_context ioc;

        client_ptr clientp{ std::make_unique<TcpClient>(ioc, m_args_ap, m_trc_ap) };
        clientp->connect(t_port);

        if (clientp->is_connected())
        {
            clientp = process_data(std::move(clientp));
            clientp->disconnect();
        }

        add_service(clientp->svcinfo());
        set_status(t_port, TaskStatus::complete);

        clientp->close();
    });
}

/**
* @brief  Write a scan progress summary to the standard output stream
*         if any user keystrokes were detected.
*/
void scan::TcpScanner::print_progress() const
{
    if (_kbhit())
    {
        if (calc_progress() > 0.0)
        {
            stdu::info(scan_progress());
        }

        while (_kbhit())
        {
            const int discard{ _getch() };
        }
    }
}

/**
* @brief  Write the scan report to the standard output stream.
*/
void scan::TcpScanner::print_report(const SvcTable &t_table) const
{
    // Display JSON scan report
    if (out_json && out_path.empty())
    {
        const string title{ algo::underline("JSON Scan Results", Color::green) };
        const json_value_t json{ JsonUtil::scan_report(t_table, m_timer, out_path) };

        std::cout << algo::concat(&LF[0], title, &LF[0])
                  << algo::concat(JsonUtil::prettify(json), &LF[0], &LF[0]);
    }
    else  // Display text scan report
    {
        const bool include_curl{ m_args_ap.load()->curl && !t_table.empty() };
        std::cout << scan_report(t_table, true, include_curl) << &LF[0];
    }
}

/**
* @brief  Stop the underlying scan timer and display the scan results.
*         Optionally saves the scan results to a local file.
*/
void scan::TcpScanner::scan_shutdown()
{
    m_timer.stop();

    sstream out_stream;
    const SvcTable table{ target.name(), m_services };

    print_report(table);

    // Format scan results as JSON
    if (out_json)
    {
        const json_value_t json{ JsonUtil::scan_report(table, m_timer, out_path) };
        out_stream << JsonUtil::prettify(json) << &LF[0];
    }
    else if (!out_path.empty())
    {
        const string title{ ArgParser::app_title("Scan Report") };
        out_stream << algo::concat(title, &LF[0], scan_report(table, false, true));
    }

    // Save the scan results to a file
    if (!out_path.empty())
    {
        FileStream::write(out_path, out_stream.str());
    }
}

/**
* @brief  Start the underlying scan timer and display the scan startup message.
*/
void scan::TcpScanner::scan_startup()
{
    const List<string> ports_list{ algo::str_vector(ports, 7) };
    string ports_str{ ports_list.join(", ") };

    // Indicate that not all ports are shown
    if (ports_list.size() < ports.size())
    {
        const size_t delta{ ports.size() - ports_list.size() };
        ports_str += algo::fstr(" ... (% not shown)", delta);
    }

    const string time_lbl{ stdu::colorize("Time  ", Color::green) };
    const string target_lbl{ stdu::colorize("Target", Color::green) };
    const string ports_lbl{ stdu::colorize("Ports ", Color::green) };

    const string title{ algo::underline(ArgParser::app_title(), Color::green, '=') };
    const string beg_timestamp{ Timer::timestamp(m_timer.start()) };

    std::cout << algo::concat(title, &LF[0])
              << algo::fstr("% : %%", time_lbl, beg_timestamp, &LF[0])
              << algo::fstr("% : %%", target_lbl, target, &LF[0])
              << algo::fstr("% : %%", ports_lbl, ports_str, &LF[0]);

    if (verbose)
    {
        std::cout << &LF[0];
    }
}

/**
* @brief  Set a task execution status in the underlying task status map.
*         Locks the underlying status map mutex.
*/
void scan::TcpScanner::set_status(const uint_t &t_port, const TaskStatus &t_status)
{
    std::scoped_lock lock{ m_statuses_mtx };
    m_statuses[t_port] = t_status;
}

/**
* @brief  Get the number of completed port scan thread pool tasks.
*         Locks the underlying status map mutex.
*/
size_t scan::TcpScanner::completed_tasks() const
{
    auto filter_pred = [](const status_t &l_pair) -> bool
    {
        return l_pair.second == TaskStatus::complete;
    };
    size_t fin_count{ 0 };

    std::scoped_lock lock{ m_statuses_mtx };
    ranges::filter_view results{ views::filter(m_statuses, filter_pred) };

    ranges::for_each(results, [&fin_count](const status_t &) { ++fin_count; });

    return fin_count;
}

/**
* @brief  Calculate the current scan progress percentage.
*/
double scan::TcpScanner::calc_progress() const
{
    size_t discard{ 0 };
    return calc_progress(discard);
}

/**
* @brief  Calculate the current scan progress percentage. Locks
*         the underlying port list mutex and sets the given task count
*         reference to the total number of completed scan tasks.
*/
double scan::TcpScanner::calc_progress(size_t &t_completed) const
{
    t_completed = completed_tasks();

    double percentage{ 0.0 };
    std::scoped_lock lock{ m_ports_mtx };

    if (ports.size() > 0)
    {
        percentage = static_cast<double>(t_completed) / ports.size();
    }
    return percentage;
}

/**
* @brief  Process the inbound and outbound socket stream data.
*/
scan::TcpScanner::client_ptr &&scan::TcpScanner::process_data(client_ptr &&t_clientp)
{
    if (t_clientp == nullptr)
    {
        throw NullPtrEx{ "t_clientp" };
    }

    if (!t_clientp->is_connected())
    {
        throw LogicEx{ "TcpScanner::process_data", "TCP client must be connected" };
    }

    TcpClient::buffer_t buffer{ CHAR_NULL };
    SvcInfo &svc_info{ t_clientp->svcinfo() };

    const size_t bytes_read{ t_clientp->recv(buffer) };
    HostState state{ t_clientp->host_state() };

    // Parse banner or probe HTTP information
    if (state == HostState::open)
    {
        const string recv_data{ string_view(&buffer[0], bytes_read) };

        if (!recv_data.empty())
        {
            svc_info.parse(recv_data);
            net::update_svc(*m_trc_ap.load(), svc_info, state);
        }

        if (m_args_ap.load()->curl || recv_data.empty())
        {
            t_clientp = probe_http(std::move(t_clientp), state);
        }
    }
    net::update_svc(*m_trc_ap.load(), svc_info, state);

    return std::move(t_clientp);
}

/**
* @brief  Get a summary of the current scan progress.
*         Locks the underlying status map mutex.
*/
std::string scan::TcpScanner::scan_progress() const
{
    size_t completed{ 0 };
    double percentage{ calc_progress(completed) };

    std::scoped_lock lock{ m_ports_mtx };
    const size_t remaining{ ports.size() - completed };

    const string progress = algo::fstr("Approximately %\\% complete (% % remaining)",
                                       percentage * 100.0,
                                       remaining,
                                       remaining == 1 ? "port" : "ports");
    return progress;
}

/**
* @brief  Get a report of the scan results in the given service table.
*/
std::string scan::TcpScanner::scan_report(const SvcTable &t_table,
                                          const bool &t_colorize,
                                          const bool &t_inc_curl) const {
    sstream stream;
    const string summary{ scan_summary(t_colorize) };

    const bool include_curl{ m_args_ap.load()->curl && !t_table.empty() };
    const string table_str{ t_table.str(t_colorize, include_curl) };

    stream << algo::concat(&LF[0], summary, &LF[0], &LF[0], table_str);

    return stream.str();
}

/**
* @brief  Get a summary of the scan results.
*/
std::string scan::TcpScanner::scan_summary(const bool &t_colorize) const
{
    string title{ "Scan Summary" };
    const size_t ln_size{ title.size() };

    // Colorize summary title
    if (t_colorize)
    {
        title = stdu::colorize(title, Color::green);
    }
    sstream stream;

    stream << algo::concat(title, &LF[0], algo::underline(ln_size, '='), &LF[0]);

    string duration_lbl{ "Duration  " };
    string beg_time_lbl{ "Start Time" };
    string end_time_lbl{ "End Time  " };

    // Colorize summary field labels
    if (t_colorize)
    {
        duration_lbl = stdu::colorize(duration_lbl, Color::green);
        beg_time_lbl = stdu::colorize(beg_time_lbl, Color::green);
        end_time_lbl = stdu::colorize(end_time_lbl, Color::green);
    }

    stream << algo::fstr("% : %%", duration_lbl, m_timer.elapsed_str(), &LF[0])
           << algo::fstr("% : %%", beg_time_lbl, m_timer.beg_timestamp(), &LF[0])
           << algo::fstr("% : %", end_time_lbl, m_timer.end_timestamp());

    // Include the report file path
    if (!out_path.empty())
    {
        string report_lbl{ "Report    " };

        if (t_colorize)
        {
            report_lbl = stdu::colorize(report_lbl, Color::green);
        }
        stream << algo::fstr("%% : '%'", &LF[0], report_lbl, out_path);
    }
    return stream.str();
}
