/*
*  scanner.cpp
*  -----------
*  Source file for an IPv4 network scanner
*/
#include <thread>
#include <conio.h>
#include "includes/containers/svctable.h"
#include "includes/inet/http/request.h"
#include "includes/inet/http/response.h"
#include "includes/inet/scanner.h"

std::string scan::Scanner::out_path;

scan::Timeout scan::Scanner::m_conn_timeout{ Socket::CONN_TIMEOUT };

/// ***
/// Initialize the object
/// ***
scan::Scanner::Scanner(const Scanner &t_scanner)
{
    operator=(t_scanner);
}

/// ***
/// Initialize the object
/// ***
scan::Scanner::Scanner(const string &t_target, const list_ui &t_ports) : Scanner()
{
    target = m_sock.addr = t_target;
    ports = t_ports;
}

/// ***
/// Destroy the object
/// ***
scan::Scanner::~Scanner()
{
    close();
}

/// ***
/// Initialize the object
/// ***
scan::Scanner::Scanner()
{
    m_sock = INVALID_SOCKET;
    m_sock.connect_timeout(Socket::CONN_TIMEOUT);
}

/// ***
/// Assignment operator overload
/// ***
scan::Scanner &scan::Scanner::operator=(const Scanner &t_scanner)
{
    m_sock = t_scanner.m_sock;
    m_timer = t_scanner.m_timer;
    m_services = t_scanner.m_services;

    target = t_scanner.target;
    ports = t_scanner.ports;

    return *this;
}

/// ***
/// Set the scanner connection timeout duration
/// ***
void scan::Scanner::connect_timeout(const Timeout &t_timeout)
{
    m_conn_timeout = t_timeout;
}

/// ***
/// Begin network scan against the target host
/// ***
void scan::Scanner::scan()
{
    // Initialize use of WinSock DLL
    if (net::wsa_startup(target) != NO_ERROR)
    {
        m_sock = INVALID_SOCKET;
        return;
    }

    // Invalid network ports
    if (!net::valid_port(ports))
    {
        throw ArgEx("m_ports", "Invalid port number");
    }

    const vector_s ports_vect{ Util::to_vector_s<uint>(ports, 7) };
    string ports_str{ list_s::join(ports_vect, ", ") };

    // Indicate that not all ports are shown
    if (ports_vect.size() < ports.size())
    {
        ports_str += "...";
    }

    m_timer.start();

    // Print scan start message
    std::cout << Util::fstr("Beginning SvcScan (%)", parser::REPO)  << stdu::LF
              << "Time: "   << Timer::timestamp(m_timer.beg_time()) << stdu::LF
              << "Target: " << target                               << stdu::LF
              << "Ports: '" << ports_str << "'"                     << stdu::LF;

    if (parser::verbose)
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

    const SvcTable table(target, m_services);
    const string summary{ net::scan_summary(target, m_timer, out_path) };

    std::cout << stdu::LF
        << summary << stdu::LF << stdu::LF
        << table   << stdu::LF;

    // Write scan results to output file
    if (!out_path.empty())
    {
        FileStream fs{ out_path, fstream::out | fstream::trunc };
        const string header{ Util::fstr("SvcScan (%) scan report", parser::REPO) };

        fs << header   << stdu::LF << stdu::LF
            << summary << stdu::LF << stdu::LF
            << table;

        fs.close();
    }
    net::wsa_cleanup();
}

/// ***
/// Close underlying socket and set its handle to default
/// ***
void scan::Scanner::close()
{
    if (m_sock.valid())
    {
        m_sock.close();
    }
}

/// ***
/// Receive socket stream data and process data received
/// ***
void scan::Scanner::process_data(const bool &t_close_sock)
{
    if (!m_sock.valid())
    {
        throw LogicEx("Scanner::scan_port", "Invalid underlying socket");
    }
    string recv_buffer;

    // Read inbound socket data
    const HostState hs{ m_sock.recv(recv_buffer, 1) };

    SvcInfo si{ m_sock.get_svcinfo() };

    // Parse and process socket data
    if (hs == HostState::open)
    {
        // Probe HTTP version information
        if (recv_buffer.empty())
        {
            const Request request{ Request::HEAD, target };
            const Response response{ m_sock.send(request) };

            // Update HTTP service information
            if (response.valid())
            {
                const vector_s old_subs{ "_", "/" };

                si.banner = Util::replace(response.get_server(), old_subs, " ");
                si.info = si.banner;
                si.service = Util::fstr("http (%)", response.version);
            }
        }
        else  // Parse TCP banner data
        {
            si.parse(recv_buffer);
        }
    }

    m_services.add(net::update_svc(si, hs));

    // Close connection socket
    if (t_close_sock)
    {
        m_sock.close();
    }
}

/// ***
/// Scan the specified TCP network port
/// ***
void scan::Scanner::scan_port(const uint &t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx("t_port", "Invalid port number specified");
    }

    // Connect to the remote port
    const bool connected{ m_sock.connect(target, t_port) };

    connected ? process_data() : m_services.add(m_sock.get_svcinfo());
    m_sock.close();
}

/// ***
/// Display a scan progress summary if a user keystroke was detected
/// ***
void scan::Scanner::show_progress(const uint &t_next_port,
                                  const size_t &t_start_pos,
                                  const bool &t_first) const {
    if (_kbhit())
    {
        if (!t_first)
        {
            stdu::info(net::scan_progress(t_next_port, ports, t_start_pos));
        }

        // Clear standard input buffer
        while (_kbhit())
        {
            const int discard{ _getch() };
        }
    }
}
