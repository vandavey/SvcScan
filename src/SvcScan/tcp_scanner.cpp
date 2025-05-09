/*
* @file
*     tcp_scanner.cpp
* @brief
*     Source file for an IPv4 TCP network scanner.
*/
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <boost/asio/io_context.hpp>
#include "includes/concepts/concepts.h"
#include "includes/console/util.h"
#include "includes/errors/arg_ex.h"
#include "includes/errors/null_ptr_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/file_system/file.h"
#include "includes/inet/net.h"
#include "includes/inet/scanners/tcp_scanner.h"
#include "includes/inet/sockets/host_state.h"
#include "includes/resources/resource.h"
#include "includes/utils/const_defs.h"
#include "includes/utils/json.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Initialize the object.
*/
scan::TcpScanner::TcpScanner(TcpScanner&& t_scanner) noexcept
    : m_io_ctx{t_scanner.m_io_ctx}
{
    *this = std::move(t_scanner);
}

/**
* @brief
*     Initialize the object.
*/
scan::TcpScanner::TcpScanner(io_context_t& t_io_ctx, shared_ptr<Args> t_argsp)
    : m_io_ctx{t_io_ctx}, m_pool{t_argsp->threads}
{
    m_rc_ap = std::make_shared<TextRc>(CSV_DATA);
    parse_argsp(t_argsp);
}

/**
* @brief
*     Move assignment operator overload.
*/
scan::TcpScanner& scan::TcpScanner::operator=(TcpScanner&& t_scanner) noexcept
{
    if (this != &t_scanner)
    {
        scoped_lock lock{m_ports_mtx, m_services_mtx, m_statuses_mtx};

        m_args_ap = std::move(t_scanner.m_args_ap.load());
        m_rc_ap = std::move(t_scanner.m_rc_ap.load());
        m_services = std::move(t_scanner.m_services);
        m_statuses = std::move(t_scanner.m_statuses);
        m_timeout = std::move(t_scanner.m_timeout);
        m_timer = std::move(t_scanner.m_timer);
        m_uri = std::move(t_scanner.m_uri);

        out_json = t_scanner.out_json.load();
        out_path = std::move(t_scanner.out_path);
        ports = std::move(t_scanner.ports);
        target = std::move(t_scanner.target);
        verbose = t_scanner.verbose.load();
    }
    return *this;
}

/**
* @brief
*     Perform the network service scan against the
*     target. Locks the underlying port list mutex.
*/
void scan::TcpScanner::scan()
{
    if (!target.valid())
    {
        throw RuntimeEx{INVALID_TARGET_MSG, "TcpScanner::scan"};
    }

    // Post scan tasks to the thread pool
    {
        scoped_lock lock{m_ports_mtx};

        if (!net::valid_port(ports))
        {
            throw RuntimeEx{INVALID_PORTS_MSG, "TcpScanner::scan"};
        }
        scan_startup();

        for (const port_t& port : ports)
        {
            post_port_scan(port);
        }
    }

    m_pool.wait();
    scan_shutdown();
}

/**
* @brief
*     Block execution until all outstanding scan tasks in
*     the underlying thread pool have completed executing.
*/
void scan::TcpScanner::wait()
{
    m_pool.wait();
}

/**
* @brief
*     Add service information to the underlying service
*     list. Locks the underlying service list mutex.
*/
void scan::TcpScanner::add_service(const SvcInfo& t_info)
{
    scoped_lock lock{m_services_mtx};
    m_services.emplace_back(t_info);
}

/**
* @brief
*     Parse information from the given command-line arguments
*     smart pointer. Locks the underlying port list mutex.
*/
void scan::TcpScanner::parse_argsp(shared_ptr<Args> t_argsp)
{
    m_args_ap = t_argsp;
    m_timeout = t_argsp->timeout;
    m_uri = t_argsp->uri;

    out_json = t_argsp->out_json;
    out_path = t_argsp->out_path;
    target = t_argsp->target;
    verbose = t_argsp->verbose;

    {
        scoped_lock lock{m_ports_mtx};

        for (const uint_t& port : ports = t_argsp->ports)
        {
            set_status(port, TaskStatus::not_started);
        }
    }
}

/**
* @brief
*     Create a new port scan task and submit it
*     to the underlying thread pool for execution.
*/
void scan::TcpScanner::post_port_scan(port_t t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{INVALID_PORTS_MSG, "t_port"};
    }

    if (!target.valid())
    {
        throw RuntimeEx{INVALID_TARGET_MSG, "TcpScanner::post_port_scan"};
    }

    // Post a new scan task to the thread pool
    m_pool.post([this, t_port]() mutable -> void
    {
        print_progress();
        set_status(t_port, TaskStatus::executing);

        io_context_t io_ctx;

        client_ptr_t clientp{std::make_unique<TcpClient>(io_ctx, m_args_ap, m_rc_ap)};
        clientp->connect(t_port);

        if (clientp->is_connected())
        {
            process_data(clientp);
            clientp->disconnect();
        }

        add_service(clientp->svcinfo());
        set_status(t_port, TaskStatus::complete);

        clientp->close();
    });
}

/**
* @brief
*     Write a scan progress summary to the standard
*     output stream if any user keystrokes were detected.
*/
void scan::TcpScanner::print_progress() const
{
    if (util::key_pressed())
    {
        if (calc_progress() > 0.0)
        {
            util::successf(scan_progress());
        }
        util::clear_keys();
    }
}

/**
* @brief
*     Write the scan report to the standard output stream.
*/
void scan::TcpScanner::print_report(const SvcTable& t_table) const
{
    std::cout << algo::concat(LF, scan_summary(true), LF, LF);

    // Print JSON scan report
    if (out_json && out_path.empty())
    {
        std::cout << json_report(t_table, true, true) << LF;
    }
    else  // Print text scan report
    {
        std::cout << algo::wrap(t_table.str(true), util::console_width()) << LF;
    }
}

/**
* @brief
*     Save the scan report to a local text or JSON file.
*/
void scan::TcpScanner::save_report(const SvcTable& t_table) const
{
    sstream output_stream;

    if (out_json)
    {
        output_stream << json_report(t_table);
    }
    else  // Save text scan report
    {
        output_stream << util::app_title("Scan Report") << LF
                      << algo::concat(LF, scan_summary(), LF, LF)
                      << algo::wrap(t_table.str(false));
    }

    File::write(out_path, output_stream.str());
}

/**
* @brief
*     Stop the underlying scan timer and display the scan
*     results. Optionally saves the scan results to a local file.
*/
void scan::TcpScanner::scan_shutdown()
{
    m_timer.stop();
    const SvcTable table{target.name(), m_args_ap.load(), m_services};

    print_report(table);

    if (!out_path.empty())
    {
        save_report(table);
    }
    util::clear_keys();
}

/**
* @brief
*     Start the underlying scan timer and display the scan startup message.
*/
void scan::TcpScanner::scan_startup()
{
    const List<string> ports_list{algo::str_vector(ports, 7)};
    string ports_str{ports_list.join(", ")};

    // Indicate that not all ports are shown
    if (ports_list.size() < ports.size())
    {
        ports_str += algo::fstr("... (% not shown)", ports.size() - ports_list.size());
    }

    std::cout << util::fmt_title(util::app_title(), true)         << LF
              << util::fmt_field("Time  ", m_timer.start(), true) << LF
              << util::fmt_field("Target", target, true)          << LF
              << util::fmt_field("Ports ", ports_str, true)       << LF;

    // Separate message and connection statuses
    if (verbose)
    {
        std::cout << LF;
    }
}

/**
* @brief
*     Set a task execution status in the underlying task
*     status map. Locks the underlying status map mutex.
*/
void scan::TcpScanner::set_status(port_t t_port, TaskStatus t_status)
{
    scoped_lock lock{m_statuses_mtx};
    m_statuses[t_port] = t_status;
}

/**
* @brief
*     Get the number of completed port scan thread pool
*     tasks. Locks the underlying status map mutex.
*/
size_t scan::TcpScanner::completed_tasks() const
{
    return ranges::count_if(m_statuses, [](const Pair auto& l_pair) -> bool
    {
        return l_pair.second == TaskStatus::complete;
    });
}

/**
* @brief
*     Calculate the current scan progress percentage.
*/
double scan::TcpScanner::calc_progress() const
{
    size_t discard{0_sz};
    return calc_progress(discard);
}

/**
* @brief
*     Calculate the current scan progress percentage. Locks
*     the underlying port list mutex and sets the given task count
*     reference to the total number of completed scan tasks.
*/
double scan::TcpScanner::calc_progress(size_t& t_completed) const
{
    t_completed = completed_tasks();

    double percentage{0.0};
    scoped_lock lock{m_ports_mtx};

    if (ports.size() > 0)
    {
        percentage = static_cast<double>(t_completed) / ports.size();
    }
    return percentage;
}

/**
* @brief
*     Process the inbound and outbound socket stream data.
*/
scan::TcpScanner::client_ptr_t& scan::TcpScanner::process_data(client_ptr_t& t_clientp)
{
    if (t_clientp == nullptr)
    {
        throw NullPtrEx{"t_clientp"};
    }

    if (!t_clientp->is_connected())
    {
        throw LogicEx{CLIENT_DISCONNECTED_MSG, "TcpScanner::process_data"};
    }

    TcpClient::buffer_t buffer{CHAR_NULL};
    SvcInfo& svc_info{t_clientp->svcinfo()};

    const size_t bytes_read{t_clientp->recv(buffer)};
    HostState state{t_clientp->host_state()};

    // Parse banner or probe HTTP information
    if (state == HostState::open)
    {
        const string recv_data(&buffer[0], bytes_read);

        if (!recv_data.empty())
        {
            svc_info.parse_banner(recv_data);
            net::update_svc(*m_rc_ap.load(), svc_info, state);
        }

        if (m_args_ap.load()->curl || recv_data.empty())
        {
            probe_http(t_clientp);
        }
    }
    net::update_svc(*m_rc_ap.load(), svc_info, state);

    return t_clientp;
}

/**
* @brief
*     Get a JSON report of the scan results in the given service table.
*/
std::string scan::TcpScanner::json_report(const SvcTable& t_table,
                                          bool t_colorize,
                                          bool t_include_title)
    const
{
    sstream stream;
    const json::object_t report{json::scan_report(t_table, m_timer, out_path)};

    if (t_include_title)
    {
        stream << util::fmt_title("Target", t_table.addr(), t_colorize) << LF;
    }
    stream << json::prettify(report) << LF;

    return stream.str();
}

/**
* @brief
*     Get a summary of the current scan progress. Locks the underlying status map mutex.
*/
std::string scan::TcpScanner::scan_progress() const
{
    size_t completed{0_sz};
    double percentage{calc_progress(completed)};

    scoped_lock lock{m_ports_mtx};
    const size_t remaining{ports.size() - completed};

    const string progress = algo::fstr("Approximately %\\% complete (% % remaining)",
                                       percentage * 100.0,
                                       remaining,
                                       remaining == 1 ? "port" : "ports");
    return progress;
}

/**
* @brief
*     Get a summary of the scan results. Optionally include the
*     command-line executable path and argument information.
*/
std::string scan::TcpScanner::scan_summary(bool t_colorize, bool t_include_cmd) const
{
    sstream stream;

    stream << util::fmt_title("Scan Summary", t_colorize)                     << LF
           << util::fmt_field("Duration  ", m_timer.elapsed(), t_colorize)    << LF
           << util::fmt_field("Start Time", m_timer.start_time(), t_colorize) << LF
           << util::fmt_field("End Time  ", m_timer.end_time(), t_colorize);

    // Include report file path
    if (!out_path.empty())
    {
        const string out_path{m_args_ap.load()->quoted_out_path()};
        stream << LF << util::fmt_field("Report    ", out_path, t_colorize);
    }

    // Include command-line details
    if (t_include_cmd)
    {
        const string exe_path{m_args_ap.load()->quoted_exe_path()};
        const string args_str{m_args_ap.load()->quoted_argv()};

        stream << LF << util::fmt_field("Executable", exe_path, t_colorize)
               << LF << util::fmt_field("Arguments ", args_str, t_colorize);
    }

    return stream.str();
}
