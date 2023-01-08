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

        for (const port_t &port : ports)
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
void scan::TcpScanner::post_port_scan(const port_t &t_port)
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
        std::cout << algo::concat(&LF[0], scan_summary(true), &LF[0], &LF[0])
                  << algo::concat(json_report(t_table, true, true), &LF[0]);
    }
    else  // Display text scan report
    {
        std::cout << text_report(t_table, true) << &LF[0];
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
    const SvcTable table{ target.name(), m_args_ap.load(), m_services };

    print_report(table);

    if (!out_json && !out_path.empty())
    {
        out_stream << ArgParser::app_title("Scan Report") << &LF[0]
                   << text_report(table, false, true);
    }
    else if (out_json)
    {
        out_stream << json_report(table);
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
    const string beg_time{ Timer::timestamp(m_timer.start()) };

    std::cout << stdu::hdr_title(ArgParser::app_title(), true)
              << algo::concat(stdu::title("Time  ", beg_time, true), &LF[0])
              << algo::concat(stdu::title("Target", target, true), &LF[0])
              << algo::concat(stdu::title("Ports ", ports_str, true), &LF[0]);

    if (verbose)
    {
        std::cout << &LF[0];
    }
}

/**
* @brief  Set a task execution status in the underlying task status map.
*         Locks the underlying status map mutex.
*/
void scan::TcpScanner::set_status(const port_t &t_port, const TaskStatus &t_status)
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
        const string recv_data(&buffer[0], bytes_read);

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
* @brief  Get a JSON report of the scan results in the given service table.
*/
std::string scan::TcpScanner::json_report(const SvcTable &t_table,
                                          const bool &t_colorize,
                                          const bool &t_inc_title) const {
    sstream stream;
    const json_value_t report{ json::scan_report(t_table, m_timer, out_path) };

    if (t_inc_title)
    {
        stream << stdu::hdr_title("Target", t_table.addr(), t_colorize);
    }
    stream << json::prettify(report) << &LF[0];

    return stream.str();
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
* @brief  Get a summary of the scan results. Optionally include the
*         command-line executable path and argument information.
*/
std::string scan::TcpScanner::scan_summary(const bool &t_colorize,
                                           const bool &t_inc_cmd) const {

    const string duration{ m_timer.elapsed_str() };
    const string beg_time{ m_timer.beg_timestamp() };
    const string end_time{ m_timer.end_timestamp() };

    sstream stream;

    stream << stdu::hdr_title("Scan Summary", t_colorize)
           << algo::concat(stdu::title("Duration  ", duration, t_colorize), &LF[0])
           << algo::concat(stdu::title("Start Time", beg_time, t_colorize), &LF[0])
           << stdu::title("End Time  ", end_time, t_colorize);

    // Include the report file path
    if (!out_path.empty())
    {
        const string out_path{ m_args_ap.load()->quoted_out_path() };
        stream << &LF[0] << stdu::title("Report    ", out_path, t_colorize);
    }

    // Include the command-line info
    if (t_inc_cmd)
    {
        const string exe_path{ m_args_ap.load()->quoted_exe_path() };
        const string args_str{ m_args_ap.load()->quoted_argv() };

        stream << &LF[0] << stdu::title("Executable", exe_path, t_colorize)
               << &LF[0] << stdu::title("Arguments ", args_str, t_colorize);
    }

    return stream.str();
}

/**
* @brief  Get a plain text report of the scan results in the given service table.
*/
std::string scan::TcpScanner::text_report(const SvcTable &t_table,
                                          const bool &t_colorize,
                                          const bool &t_inc_cmd) const {
    sstream stream;

    const string summary{ scan_summary(t_colorize, t_inc_cmd) };
    const bool inc_curl{ m_args_ap.load()->curl && !t_table.empty() };

    stream << algo::concat(&LF[0], summary, &LF[0], &LF[0])
           << t_table.str(t_colorize, inc_curl, verbose.load());

    return stream.str();
}
