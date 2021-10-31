/*
*  scanner.cpp
*  -----------
*  Source file for an IPv4 network scanner
*/
#include <conio.h>
#include "includes/containers/svctable.h"
#include "includes/inet/scanner.h"

scan::AutoProp<std::string> scan::Scanner::out_path;

timeval scan::Scanner::m_timeout{ 3, 500 };

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
scan::Scanner::Scanner(const string &t_target, const list_ui &t_ports)
{
    m_target = t_target;
    m_ports = t_ports;
    m_sock = t_target;

    target = &m_target;
    ports = &m_ports;
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
}

scan::Scanner &scan::Scanner::operator=(const Scanner &t_scanner)
{
    m_sock = t_scanner.m_sock;
    m_timer = t_scanner.m_timer;
    m_target = t_scanner.m_target;
    m_ports = t_scanner.m_ports;
    m_services = t_scanner.m_services;

    target = &m_target;
    ports = &m_ports;

    return *this;
}

/// ***
/// Set the default connection timeout duration
/// ***
void scan::Scanner::set_timeout(const uint &t_sec, const uint &t_ms)
{
    m_timeout = { static_cast<long>(t_sec), static_cast<long>(t_ms) };
}

/// ***
/// Begin network scan against the target host
/// ***
void scan::Scanner::scan()
{
    // Initialize use of WinSock DLL
    if (net::wsa_startup(m_target) != NO_ERROR)
    {
        m_sock = INVALID_SOCKET;
        return;
    }

    // Invalid network ports
    if (!net::valid_port(m_ports))
    {
        throw ArgEx("m_ports", "Invalid port number");
    }

    const vector_s ports_vect{ Util::to_vector_s<uint>(m_ports, 7) };
    string ports_str{ list_s::join(ports_vect, ", ") };

    // Indicate that not all ports are shown
    if (ports_vect.size() < m_ports.size())
    {
        ports_str += "...";
    }

    m_timer.start();

    // Print scan start message
    std::cout << Util::fstr("Beginning SvcScan (%)", Parser::REPO)  << stdu::LF
              << "Time: "   << Timer::timestamp(m_timer.beg_time()) << stdu::LF
              << "Target: " << m_target                             << stdu::LF
              << "Ports: '" << ports_str << "'"                     << stdu::LF;

    if (Parser::verbose)
    {
        std::cout << stdu::LF;
    }

    // Connect to each port in underlying ports list
    for (size_t i{ 0 }; i < m_ports.size(); i++)
    {
        show_progress(m_ports[i], i, i == 0);
        scan_port(m_ports[i]);
    }

    m_timer.stop();

    const SvcTable table(m_target, m_services);
    const string summary{ net::scan_summary(m_target, m_timer, out_path) };

    std::cout << stdu::LF
        << summary << stdu::LF << stdu::LF
        << table   << stdu::LF;

    // Write scan results to output file
    if (!out_path.get().empty())
    {
        FileStream fs{ out_path, fstream::out | fstream::trunc };
        const string header{ Util::fstr("SvcScan (%) scan report", Parser::REPO) };

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
    char buffer[Socket::BUFFER_SIZE]{ 0 };

    // Read inbound socket data
    const HostState hs{ m_sock.recv(buffer) };

    const string buffstr{ buffer };
    SvcInfo si{ m_sock.get_svcinfo() };

    // Parse and process socket data
    if ((hs == HostState::open) && !buffstr.empty())
    {
        si.parse(buffer);
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
    const bool connected{ m_sock.connect(m_target, t_port) };

    connected ? process_data() : m_services.add(m_sock.get_svcinfo());
    m_sock.close();
}

/// ***
/// Display a scan progress summary if a user keystroke was detected
/// ***
void scan::Scanner::show_progress(const uint &t_next_port,
                                  const size_t &t_start_pos,
                                  const bool &t_first) const {
    if (_kbhit() && !t_first)
    {
        stdu::info(net::scan_progress(t_next_port, m_ports, t_start_pos));
    }

    // Clear standard input buffer
    if (_kbhit())
    {
        while (_kbhit())
        {
            const int discard{ _getch() };
        }
    }
}
