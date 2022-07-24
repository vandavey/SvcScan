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
    *this = std::forward<Scanner>(t_scanner);
}

/**
* @brief  Initialize the object.
*/
scan::Scanner::Scanner(io_context &t_ioc, const Args &t_args) : m_ioc(t_ioc)
{
    m_csv_rc = CSV_DATA;
    m_clientp = std::make_unique<TcpClient>(m_ioc, t_args, &m_csv_rc);

    parse_args(t_args);
}

/**
* @brief  Destroy the object.
*/
scan::Scanner::~Scanner()
{
    if (m_clientp->is_open())
    {
        error_code discard_ecode;
        m_clientp->socket().close(discard_ecode);
    }
}

/**
* @brief  Move assignment operator overload.
*/
scan::Scanner &scan::Scanner::operator=(Scanner &&t_scanner) noexcept
{
    if (this != &t_scanner)
    {
        m_args = t_scanner.m_args;
        m_clientp = std::move(t_scanner.m_clientp);
        m_conn_timeout = t_scanner.m_conn_timeout;
        m_csv_rc = std::move(t_scanner.m_csv_rc);
        m_http_uri = t_scanner.m_http_uri;
        m_services = t_scanner.m_services;
        m_timer = t_scanner.m_timer;

        out_path = t_scanner.out_path;
        ports = t_scanner.ports;
        target = t_scanner.target;
        verbose = t_scanner.verbose;
    }
    return *this;
}

/**
* @brief  Close the underlying TCP client.
*/
void scan::Scanner::close()
{
    if (m_clientp->is_open() && m_clientp->is_connected())
    {
        m_clientp->disconnect();
    }
    else if (m_clientp->is_open())
    {
        m_clientp->close();
    }
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

    startup();  // Display startup message

    // Connect to each port in underlying ports list
    for (port_iterator it{ ports.begin() }; it != ports.end(); ++it)
    {
        show_progress(it);
        scan_port(*it);
    }

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
* @brief  Parse information from the given command-line arguments.
*/
void scan::Scanner::parse_args(const Args &t_args)
{
    m_args = t_args;
    m_conn_timeout = m_args.timeout;
    m_http_uri = m_args.uri;

    out_path = m_args.out_path;
    ports = m_args.ports;
    target = m_args.target;
    verbose = m_args.verbose;
}

/**
* @brief  Perform HTTP communications to identify server information.
*/
void scan::Scanner::probe_http(SvcInfo &t_si, HostState &t_hs)
{
    if (!m_clientp->is_connected())
    {
        throw LogicEx{ "Scanner::process_data", "TCP client must be connected" };
    }

    const Request request{ http::verb::head, target, m_http_uri };
    const Response response{ m_clientp->request(request) };

    // Update HTTP service information
    if (response.valid())
    {
        t_hs = HostState::open;
        t_si.service = Util::fstr("http (%)", response.httpv.num_str());

        t_si.banner = Util::replace(response.server(),
                                    vector<string>{ "_", "/" },
                                    " ");
        t_si.summary = t_si.banner;
    }
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
* @brief  Scan the specified network port.
*/
void scan::Scanner::scan_port(const uint &t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number specified" };
    }
    m_clientp->connect({ target, t_port });

    if (m_clientp->is_connected())
    {
        process_data();
    }

    m_services.add(m_clientp->svcinfo());
    close();
}

/**
* @brief  Display a scan progress summary if any user keystroke were detected.
*/
void scan::Scanner::show_progress(const port_iterator &t_port_it) const
{
    if (_kbhit())
    {
        if (t_port_it != ports.begin())
        {
            stdu::info(progress(t_port_it));
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
* @brief  Read and process the inbound socket stream data. Returns true
*         if inbound socket data was successfully processed.
*/
bool scan::Scanner::process_data()
{
    if (!m_clientp->is_connected())
    {
        throw LogicEx{ "Scanner::process_data", "TCP client must be connected" };
    }

    char buffer[TcpClient::BUFFER_SIZE]{ '\0' };

    const size_t bytes_read{ m_clientp->recv(buffer) };
    HostState state{ m_clientp->host_state() };

    if (state == HostState::open)
    {
        const string recv_data{ std::string_view(&buffer[0], bytes_read) };

        // Probe HTTP version information
        if (recv_data.empty())
        {
            probe_http(m_clientp->svcinfo(), state);
        }
        else  // Parse TCP banner data
        {
            m_clientp->svcinfo().parse(recv_data);
        }
    }
    net::update_svc(m_csv_rc, m_clientp->svcinfo(), state);

    return true;
}

/**
* @brief  Get a summary of the current scan progress.
*/
std::string scan::Scanner::progress(const port_iterator &t_port_it) const
{
    if (t_port_it == ports.end())
    {
        throw ArgEx{ "t_port_it", "Invalid port iterator specified" };
    }

    double percent{ 0 };
    const size_t iter_delta{ Util::distance(ports, t_port_it) };

    if (iter_delta != 0)
    {
        percent = (double(iter_delta) / ports.size()) * 100;
    }

    const size_t rem_count{ ports.size() - iter_delta };
    const string rem_msg{ (rem_count == 1) ? "port remaining" : "ports remaining" };

    return Util::fstr("Scan %\\% completed (% %)", percent, rem_count, rem_msg);
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
