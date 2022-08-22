/*
*  scanner.cpp
*  -----------
*  Source file for an IPv4 network scanner
*/
#include <conio.h>
#include "includes/inet/http/request.h"
#include "includes/inet/scanners/scanner.h"
#include "includes/inet/sockets/tls_client.h"
#include "includes/resources/resource.h"
#include "includes/utils/arg_parser.h"

/**
* @brief  Initialize the object.
*/
scan::Scanner::Scanner(Scanner &&t_scanner) noexcept : m_ioc(t_scanner.m_ioc)
{
    *this = std::forward<this_t>(t_scanner);
}

/**
* @brief  Initialize the object.
*/
scan::Scanner::Scanner(io_context &t_ioc, shared_ptr<Args> t_argsp)
    : m_ioc(t_ioc), m_pool(t_argsp->concurrency) {

    m_trc_ap = std::make_shared<TextRc>(CSV_DATA);
    parse_argsp(t_argsp);
}

/**
* @brief  Move assignment operator overload.
*/
scan::Scanner &scan::Scanner::operator=(Scanner &&t_scanner) noexcept
{
    if (this != &t_scanner)
    {
        m_concurrency = t_scanner.m_concurrency;
        m_conn_timeout = t_scanner.m_conn_timeout;
        m_http_uri = t_scanner.m_http_uri;
        m_services = t_scanner.m_services;
        m_status_map = t_scanner.m_status_map;
        m_timer = t_scanner.m_timer;

        m_args_ap.store(std::move(t_scanner.m_args_ap));
        m_trc_ap.store(std::move(t_scanner.m_trc_ap));

        out_path = t_scanner.out_path;
        ports = t_scanner.ports;
        target = t_scanner.target;
        verbose = t_scanner.verbose;
    }
    return *this;
}

/**
* @brief  Set the scanner connection timeout duration.
*/
void scan::Scanner::connect_timeout(const Timeout &t_timeout)
{
    m_conn_timeout = t_timeout;
}

/**
* @brief  Perform the network service scan against the target.
*/
void scan::Scanner::scan()
{
    // The app should have already exited
    if (!target.is_valid())
    {
        throw RuntimeEx{ "Scanner::scan", "Invalid underlying target hostname" };
    }

    // The app should have already exited
    if (!net::valid_port(ports))
    {
        throw RuntimeEx{ "Scanner::scan", "Invalid underlying port number(s)" };
    }

    startup();

    // Submit scan tasks to thread pool
    for (const uint &port : ports)
    {
        post_port_scan(port);
    }

    m_pool.wait();
    m_timer.stop();

    const SvcTable table{ target.name(), m_services };
    const string summary_str{ summary() };

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
void scan::Scanner::wait()
{
    m_pool.wait();
}

/**
* @brief  Add service information to the underlying service list.
*         Locks the underlying service list mutex.
*/
void scan::Scanner::add_service(const SvcInfo &t_si)
{
    std::scoped_lock list_lock{ m_list_mutex };
    m_services.add(t_si);
}

/**
* @brief  Parse information from the given command-line arguments smart pointer.
*/
void scan::Scanner::parse_argsp(shared_ptr<Args> t_argsp)
{
    m_args_ap = t_argsp;
    m_concurrency = t_argsp->concurrency;
    m_conn_timeout = t_argsp->timeout;
    m_http_uri = t_argsp->uri;

    out_path = t_argsp->out_path;
    ports = t_argsp->ports;
    target = t_argsp->target;
    verbose = t_argsp->verbose;

    for (const uint &port : ports)
    {
        update_status(port, TaskStatus::not_started);
    }
}

/**
* @brief  Create a new port scan task and submit it to the underlying
*         thread pool for execution.
*/
void scan::Scanner::post_port_scan(const uint &t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number specified" };
    }

    // The app should have already exited
    if (!target.is_valid())
    {
        throw RuntimeEx{ "Scanner::post_task", "Invalid underlying target" };
    }

    // Post a new scan task to the thread pool
    m_pool.post([&, this]() mutable -> void
    {
        show_progress();
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
void scan::Scanner::save_report(const string &t_path,
                                const string &t_summary,
                                const SvcTable &t_table) {

    FileStream file_stream{ out_path, fstream::out | fstream::trunc };
    const string header{ Util::fstr("SvcScan (%) scan report", ArgParser::REPO) };

    file_stream << header    << stdu::LF << stdu::LF
                << t_summary << stdu::LF << stdu::LF
                << t_table;

    file_stream.close();
}

/**
* @brief  Display a scan progress summary if any user keystrokes were detected.
*/
void scan::Scanner::show_progress() const
{
    std::scoped_lock kb_lock{ m_kb_io_mutex };

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
* @brief  Start the underlying scan timer and display the scan startup message.
*/
void scan::Scanner::startup()
{
    const List<string> ports_list{ Util::to_str_vector<uint>(ports, 7) };
    string ports_str{ ports_list.join(", ") };

    // Indicate that not all ports are shown
    if (ports_list.size() < ports.size())
    {
        ports_str += "...";
    }

    // Print scan start message
    std::cout << Util::fstr("Beginning SvcScan (%)", ArgParser::REPO) << stdu::LF
              << "Time: "   << Timer::timestamp(m_timer.start())      << stdu::LF
              << "Target: " << target                                 << stdu::LF
              << "Ports: "  << Util::fstr("'%'", ports_str)           << stdu::LF;

    if (verbose)
    {
        std::cout << stdu::LF;
    }
}

/**
* @brief  Update a task status in the underlying task status map.
*/
void scan::Scanner::update_status(const uint &t_port, const TaskStatus &t_status)
{
    std::scoped_lock map_lock{ m_map_mutex };
    m_status_map[t_port] = t_status;
}

/**
* @brief  Get the number of completed port scan thread pool tasks.
*/
size_t scan::Scanner::completed_tasks() const
{
    std::scoped_lock container_lock{ m_list_mutex, m_map_mutex };

    size_t completed{ 0 };
    status_map map{ m_status_map };

    ranges::filter_view results = std::views::filter(map, [](const status_t &l_pair)
    {
        return l_pair.second == TaskStatus::complete;
    });
    ranges::for_each(results, [&completed](const status_t &) { ++completed; });

    return completed;
}

/**
* @brief  Calculate the current scan progress percentage.
*/
double scan::Scanner::calc_progress() const
{
    double percentage{ 0 };
    const size_t completed{ completed_tasks() };

    std::scoped_lock list_lock{ m_list_mutex };

    if (ports.size() > 0)
    {
        percentage = double(completed) / ports.size();
    }
    return percentage;
}

/**
* @brief  Read and process the inbound socket stream data.
*/
scan::Scanner::client_ptr &&scan::Scanner::process_data(client_ptr &&t_clientp)
{
    if (t_clientp == nullptr)
    {
        throw NullPtrEx{ "t_clientp" };
    }

    if (!t_clientp->is_connected())
    {
        throw LogicEx{ "Scanner::process_data", "TCP client must be connected" };
    }

    char buffer[TcpClient::BUFFER_SIZE]{ '\0' };
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
std::string scan::Scanner::progress() const
{
    double percentage{ calc_progress() };
    const size_t completed{ completed_tasks() };

    std::scoped_lock list_lock{ m_list_mutex };
    const size_t remaining{ ports.size() - completed };

    const string prog_summary = Util::fstr("Scan %\\% complete (% % remaining)",
                                           percentage * 100,
                                           remaining,
                                           remaining == 1 ? "port" : "ports");
    return prog_summary;
}

/**
* @brief  Get a summary of the scan results as a string.
*/
std::string scan::Scanner::summary() const
{
    std::stringstream sstream;
    const string title{ "Scan Summary" };

    const string beg_time{ Timer::timestamp(m_timer.beg_time()) };
    const string end_time{ Timer::timestamp(m_timer.end_time()) };

    sstream << Util::fstr("%%", title, stdu::LF)
            << Util::fstr("Duration   : %%", m_timer.elapsed_str(), stdu::LF)
            << Util::fstr("Start Time : %%", beg_time, stdu::LF)
            << Util::fstr("End Time   : %", end_time);

    // Include the report file path
    if (!out_path.empty())
    {
        sstream << Util::fstr("%Report     : '%'", stdu::LF, out_path);
    }
    return sstream.str();
}
