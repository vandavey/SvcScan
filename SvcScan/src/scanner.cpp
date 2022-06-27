/*
*  scanner.cpp
*  -----------
*  Source file for an IPv4 network scanner
*/
#include <conio.h>
#include "includes/inet/http/request.h"
#include "includes/inet/scanner.h"
#include "includes/inet/sockets/tls_client.h"
#include "includes/utils/arg_parser.h"

/**
* @brief  Initialize the object.
*/
scan::Scanner::Scanner(Scanner &&t_scanner) noexcept : m_ioc(t_scanner.m_ioc)
{
    m_args = t_scanner.m_args;
    m_clientp = std::move(t_scanner.m_clientp);
    m_conn_timeout = t_scanner.m_conn_timeout;
    m_http_uri = t_scanner.m_http_uri;
    m_services = t_scanner.m_services;
    m_timer = t_scanner.m_timer;

    out_path = t_scanner.out_path;
    ports = t_scanner.ports;
    target = t_scanner.target;
    verbose = t_scanner.verbose;
}

/**
* @brief  Initialize the object.
*/
scan::Scanner::Scanner(io_context &t_ioc, const Args &t_args) : m_ioc(t_ioc)
{
    m_clientp = std::make_unique<TcpClient>(m_ioc, t_args);
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
* @brief  Close the underlying TCP client.
*/
void scan::Scanner::close()
{
    if (m_clientp->is_open())
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

    const vector<string> ports_vect{ Util::to_str_vector<uint>(ports, 7) };
    string ports_str{ List<string>(ports_vect).join(", ") };

    // Indicate that not all ports are shown
    if (ports_vect.size() < ports.size())
    {
        ports_str += "...";
    }

    m_timer.start();

    // Print scan start message
    std::cout << Util::fstr("Beginning SvcScan (%)", ArgParser::REPO) << stdu::LF
              << "Time: "   << Timer::timestamp(m_timer.beg_time())   << stdu::LF
              << "Target: " << target                                 << stdu::LF
              << "Ports: "  << Util::fstr("'%'", ports_str)           << stdu::LF;

    if (verbose)
    {
        std::cout << stdu::LF;
    }

    // Connect to each port in underlying ports list
    for (size_t i{ 0 }; i < ports.size(); i++)
    {
        show_progress(ports[i], i, i == 0);
        scan_port(ports[i]);
    }

    m_timer.stop();

    const SvcTable table{ target.name(), m_services };
    const string summary{ scan_summary() };

    std::cout << stdu::LF
              << summary << stdu::LF << stdu::LF
              << table   << stdu::LF;

    // Save scan report to file
    if (!out_path.empty())
    {
        save_report(out_path, summary, table);
    }
}

/**
* @brief  Reinitialize the underlying client to enable/disable SSL/TLS connections.
*/
void scan::Scanner::configure_client(const bool &t_secure)
{
    if (m_clientp == nullptr)
    {
        throw LogicEx{ "Scanner::configure_client", "TCP client pointer is null" };
    }

    if (t_secure && typeid(*m_clientp) != typeid(TlsClient))
    {
        m_clientp = std::make_unique<TlsClient>(m_ioc, m_args);
    }
    else if (typeid(*m_clientp) != typeid(TcpClient))
    {
        m_clientp = std::make_unique<TcpClient>(m_ioc, m_args);
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
* @brief  Read and process the inbound socket stream data.
*/
void scan::Scanner::process_data()
{
    if (!m_clientp->is_connected())
    {
        throw LogicEx{ "Scanner::process_data", "TCP client must be connected" };
    }

    error_code ecode;
    char buffer[TcpClient::BUFFER_SIZE]{ '\0' };

    const size_t bytes_read{ m_clientp->recv(buffer, ecode) };
    HostState state{ m_clientp->host_state(ecode) };

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

    net::update_svc(m_clientp->textrc(), m_clientp->svcinfo(), state);
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

    m_clientp->is_connected() ? m_clientp->disconnect() : m_clientp->close();
}

/**
* @brief  Get the current scan progress as a string.
*/
std::string scan::Scanner::scan_progress(const uint &t_next_port,
                                         const size_t &t_start_pos) const {
    if (t_next_port == 0U)
    {
        throw NullArgEx{ "t_next_port" };
    }

    if (ports.empty())
    {
        throw LogicEx{ "Scanner::scan_progress", "No underlying port(s) specified" };
    }

    const size_t position{ ports.find(t_next_port, t_start_pos) };
    const size_t rem_num{ ports.size() - position };

    const double done_num{ static_cast<double>(position) };
    const double progress{ done_num / static_cast<double>(ports.size()) * 100 };

    std::stringstream ss;
    const string rem_str{ (rem_num == 1) ? "port remaining" : "ports remaining" };

    ss.precision(4);
    ss << Util::fstr("Scan %\\% completed (% %)", progress, rem_num, rem_str);

    return ss.str();
}

/**
* @brief  Get a summary of the scan results as a string.
*/
std::string scan::Scanner::scan_summary() const
{
    std::stringstream ss;
    const string title{ "Scan Summary" };

    const string beg_time{ Timer::timestamp(m_timer.beg_time()) };
    const string end_time{ Timer::timestamp(m_timer.end_time()) };

    ss << Util::fstr("%%", title, stdu::LF)
        << Util::fstr("Duration   : %%", m_timer.elapsed_str(), stdu::LF)
        << Util::fstr("Start Time : %%", beg_time, stdu::LF)
        << Util::fstr("End Time   : %", end_time);

    // Include the report file path
    if (!out_path.empty())
    {
        ss << Util::fstr("%Report     : '%'", stdu::LF, out_path);
    }
    return ss.str();
}

/**
* @brief  Display a scan progress summary if any user keystroke were detected.
*/
void scan::Scanner::show_progress(const uint &t_next_port,
                                  const size_t &t_start_pos,
                                  const bool &t_first) const {
    if (_kbhit())
    {
        if (!t_first)
        {
            stdu::info(scan_progress(t_next_port, t_start_pos));
        }

        // Clear entire stdin buffer
        while (_kbhit())
        {
            const int discard{ _getch() };
        }
    }
}
