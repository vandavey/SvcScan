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

        m_conn_timeout = t_scanner.m_conn_timeout;
        m_http_uri = t_scanner.m_http_uri;
        m_services = t_scanner.m_services;
        m_statuses = t_scanner.m_statuses;
        m_threads = t_scanner.m_threads;
        m_timer = t_scanner.m_timer;

        m_args_ap.store(std::move(t_scanner.m_args_ap));
        m_trc_ap.store(std::move(t_scanner.m_trc_ap));
        verbose.store(std::move(t_scanner.verbose));

        out_path = t_scanner.out_path;
        ports = t_scanner.ports;
        target = t_scanner.target;
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

        for (const uint &port : ports)
        {
            post_port_scan(port);
        }
    }

    m_pool.wait();
    m_timer.stop();

    const string summary_str{ summary() };
    const SvcTable table{ target.name(), m_services };

    std::cout << stdu::LF
              << summary_str << stdu::LF << stdu::LF
              << table       << stdu::LF;

    // Save scan report to file
    if (!out_path.empty())
    {
        save_report(out_path, summary_str, table);
    }
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
*/
void scan::TcpScanner::parse_argsp(shared_ptr<Args> t_argsp)
{
    m_args_ap = t_argsp;
    m_conn_timeout = t_argsp->timeout;
    m_http_uri = t_argsp->uri;
    m_threads = t_argsp->threads;

    out_path = t_argsp->out_path;
    target = t_argsp->target;
    verbose = t_argsp->verbose;

    {
        std::scoped_lock lock{ m_ports_mtx };

        for (const uint &port : ports = t_argsp->ports)
        {
            update_status(port, TaskStatus::not_started);
        }
    }
}

/**
* @brief  Create a new port scan task and submit it to the underlying
*         thread pool for execution.
*/
void scan::TcpScanner::post_port_scan(const uint &t_port)
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
        show_progress();
        update_status(t_port, TaskStatus::executing);

        io_context ioc;

        client_ptr clientp{ std::make_unique<TcpClient>(ioc, m_args_ap, m_trc_ap) };
        clientp->connect(t_port);

        if (clientp->is_connected())
        {
            clientp = process_data(std::move(clientp));
            clientp->disconnect();
        }

        add_service(clientp->svcinfo());
        update_status(t_port, TaskStatus::complete);

        clientp->close();
    });
}

/**
* @brief  Save the network scan results to the given file path.
*/
void scan::TcpScanner::save_report(const string &t_path,
                                   const string &t_summary,
                                   const SvcTable &t_table) {

    FileStream file_stream{ out_path, fstream::out | fstream::trunc };
    const string header{ algo::fstr("SvcScan (%) scan report", ArgParser::REPO) };

    file_stream << header    << stdu::LF << stdu::LF
                << t_summary << stdu::LF << stdu::LF
                << t_table;

    file_stream.close();
}

/**
* @brief  Start the underlying scan timer and display the scan startup message.
*/
void scan::TcpScanner::scan_startup()
{
    const List<string> ports_list{ algo::str_vector<uint>(ports, 7) };
    string ports_str{ ports_list.join(", ") };

    // Indicate that not all ports are shown
    if (ports_list.size() < ports.size())
    {
        const size_t delta{ ports.size() - ports_list.size() };
        ports_str += algo::fstr(" ... (% not shown)", delta);
    }

    std::cout << algo::fstr("Beginning SvcScan (%)", ArgParser::REPO) << stdu::LF
              << "Time: "   << Timer::timestamp(m_timer.start())      << stdu::LF
              << "Target: " << target                                 << stdu::LF
              << "Ports: "  << algo::fstr("%", ports_str)             << stdu::LF;

    if (verbose)
    {
        std::cout << stdu::LF;
    }
}

/**
* @brief  Display a scan progress summary if any user keystrokes were detected.
*/
void scan::TcpScanner::show_progress() const
{
    if (_kbhit())
    {
        if (calc_progress() > 0.0)
        {
            stdu::info(progress());
        }

        // Clear entire stdin buffer
        while (_kbhit())
        {
            const int discard{ _getch() };
        }
    }
}

/**
* @brief  Update a task status in the underlying task status map.
*/
void scan::TcpScanner::update_status(const uint &t_port, const TaskStatus &t_status)
{
    std::scoped_lock lock{ m_statuses_mtx };
    m_statuses[t_port] = t_status;
}

/**
* @brief  Get the number of completed port scan thread pool tasks.
*/
size_t scan::TcpScanner::completed_tasks() const
{
    auto filter_pred = [](const status_t &l_pair) -> bool
    {
        return l_pair.second == TaskStatus::complete;
    };
    size_t completed{ 0 };

    std::scoped_lock lock{ m_statuses_mtx };
    ranges::filter_view results{ ranges::views::filter(m_statuses, filter_pred) };

    ranges::for_each(results, [&completed](const status_t &) { ++completed; });

    return completed;
}

/**
* @brief  Calculate the current scan progress percentage.
*/
double scan::TcpScanner::calc_progress() const
{
    size_t completed{ 0 };
    return calc_progress(completed);
}

/**
* @brief  Calculate the current scan progress percentage. Sets the given
*         task count reference to the number of completed port scan tasks.
*/
double scan::TcpScanner::calc_progress(size_t &t_completed) const
{
    t_completed = completed_tasks();

    double percentage{ 0 };
    std::scoped_lock lock{ m_ports_mtx };

    if (ports.size() > 0)
    {
        percentage = static_cast<double>(t_completed) / ports.size();
    }
    return percentage;
}

/**
* @brief  Read and process the inbound socket stream data.
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

    TcpClient::buffer_t buffer{ '\0' };
    SvcInfo &svc_info{ t_clientp->svcinfo() };

    const size_t bytes_read{ t_clientp->recv(buffer) };
    HostState state{ t_clientp->host_state() };

    // Parse banner or probe HTTP information
    if (state == HostState::open)
    {
        const string recv_data{ std::string_view(&buffer[0], bytes_read) };

        if (recv_data.empty())
        {
            t_clientp = probe_http(std::move(t_clientp), state);
        }
        else  // Parse TCP banner data
        {
            svc_info.parse(recv_data);
        }
    }
    net::update_svc(*m_trc_ap.load(), svc_info, state);

    return std::move(t_clientp);
}

/**
* @brief  Get a summary of the current scan progress.
*/
std::string scan::TcpScanner::progress() const
{
    size_t completed{ 0 };
    double percentage{ calc_progress(completed) };

    std::scoped_lock lock{ m_ports_mtx };
    const size_t remaining{ ports.size() - completed };

    const string summary_fstr{ "Approximately %\\% complete (% % remaining)" };

    const string prog_summary = algo::fstr(summary_fstr,
                                           percentage * 100,
                                           remaining,
                                           remaining == 1 ? "port" : "ports");
    return prog_summary;
}

/**
* @brief  Get a summary of the scan results as a string.
*/
std::string scan::TcpScanner::summary() const
{
    std::stringstream sstream;
    const string title{ "Scan Summary" };

    const string beg_time{ Timer::timestamp(m_timer.beg_time()) };
    const string end_time{ Timer::timestamp(m_timer.end_time()) };

    sstream << algo::fstr("%%", title, stdu::LF)
            << algo::fstr("Duration   : %%", m_timer.elapsed_str(), stdu::LF)
            << algo::fstr("Start Time : %%", beg_time, stdu::LF)
            << algo::fstr("End Time   : %", end_time);

    // Include the report file path
    if (!out_path.empty())
    {
        sstream << algo::fstr("%Report     : '%'", stdu::LF, out_path);
    }
    return sstream.str();
}
