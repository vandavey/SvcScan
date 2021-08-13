/*
*  netutil.cpp
*  -----------
*  Source file for network and socket utilities
*/
#include <algorithm>
#include <ws2tcpip.h>
#include "includes/filesys/filestream.h"
#include "includes/inet/netutil.h"

#pragma comment(lib, "ws2_32.lib")

bool scan::NetUtil::m_data_read{ false };

scan::NetUtil::uint scan::NetUtil::m_wsa_call_count{ 0 };

scan::NetUtil::vector_a scan::NetUtil::m_svcvect;

/// ***
/// Format and print WSA error message to standard error stream
/// ***
void scan::NetUtil::error(const string &t_addr)
{
    error(EndPoint(t_addr));
}

/// ***
/// Format and print WSA error message to standard error stream
/// ***
void scan::NetUtil::error(const EndPoint &t_ep, const int &t_err)
{
    const int err{ (t_err == NULL) ? get_error() : t_err };

    switch (err)
    {
        case WSAHOST_NOT_FOUND:  // Name resolution error
            stdu::errorf("Unable to resolve host name '%'", t_ep.addr);
            break;
        case WSAECONNREFUSED:    // Connection refused
            stdu::errorf("Connection refused: %/tcp", t_ep.port);
            break;
        case WSAECONNRESET:      // Connection reset
            stdu::errorf("Connection forcibly closed: %/tcp", t_ep.port);
            break;
        case WSAEHOSTDOWN:       // Destination host down
            stdu::errorf("Target down or unresponsive: %/tcp", t_ep.port);
            break;
        case WSANOTINITIALISED:  // WSAStartup call missing
            stdu::error("Missing call to WSAStartup()");
            break;
        case WSAETIMEDOUT:       // Socket timeout
        case WSAEWOULDBLOCK:     // Operation incomplete
            stdu::errorf("Connection timeout: %/tcp", t_ep.port);
            break;
        default:                 // Default (error code)
            stdu::errorf("Winsock error: %", err);
            break;
    }
}

/// ***
/// Free memory being used by the underlying array vector
/// ***
void scan::NetUtil::free_info()
{
    if (m_data_read)
    {
        m_svcvect.clear();
        m_svcvect.shrink_to_fit();

        m_data_read = false;
    }
}

/// ***
/// Copy the embedded CSV data into underlying array vector
/// ***
void scan::NetUtil::load_info()
{
    if (!m_data_read)
    {
        const vector_s lines{ FileStream::read_csv_lines(Resource()) };

        // Split lines into fields
        for (const string &line : lines)
        {
            const string new_line{ Util::strip(line, '\"', false) };
            const array_s fields{ copy_n<string, 4>(Util::split(new_line, ",", 3)) };

            m_svcvect.push_back(fields);
        }
        m_data_read = true;
    }
}

/// ***
/// Determine if the given integer is a valid network port
/// ***
bool scan::NetUtil::valid_port(const int &t_port)
{
    return (t_port >= 0) && (t_port <= MAX_PORT);
}

/// ***
/// Determine if the given string is a valid network port
/// ***
bool scan::NetUtil::valid_port(const string &t_port)
{
    bool is_digit = std::all_of(t_port.begin(), t_port.end(), [](const char &l_port)
    {
        return std::isdigit(l_port);
    });
    return is_digit ? valid_port(std::stoi(t_port)) : false;
}

/// ***
/// Determine if the vector integers are valid network ports
/// ***
bool scan::NetUtil::valid_port(const vector_ui &t_ports)
{
    return std::all_of(t_ports.begin(), t_ports.end(), [](const int &l_port)
    {
        return valid_port(l_port);
    });
}

/// ***
/// Determine if socket is valid
/// ***
bool scan::NetUtil::valid_sock(const SOCKET &t_sock) noexcept
{
    if (t_sock == NULL)
    {
        return false;
    }
    return (t_sock != INVALID_SOCKET) && (t_sock != SOCKET_ERROR);
}

/// ***
/// Get the last error using WSAGetLastError
/// ***
int scan::NetUtil::get_error()
{
    return WSAGetLastError();
}

/// ***
/// Configure blocking options on underlying socket
/// ***
int scan::NetUtil::set_blocking(SOCKET &t_sock, const bool &t_do_block)
{
    if (!valid_sock(t_sock))
    {
        throw ArgEx("t_sock", "The given socket is invalid");
    }
    ulong mode{ static_cast<ulong>(t_do_block ? 0 : 1) };

    // Modify socket blocking
    return ::ioctlsocket(t_sock, FIONBIO, &mode);
}

/// ***
/// Determine if IPv4 string (dotted-quad notation) is valid
/// ***
int scan::NetUtil::valid_ip(const string &t_addr)
{
    // Don't attempt resolution (invalid/unknown format)
    if (Util::count(t_addr, '.') != 3)
    {
        return SOCKET_ERROR;
    }
    int iaddr{ static_cast<int>(sizeof(in_addr)) };

    // Convert IPv4 string to binary
    const int code{ inet_pton(AF_INET, t_addr.c_str(), &iaddr) };

    return (code == SOCKET_READY) ? 0 : 1;
}

/// ***
/// Handle WinSock required WSACleanup function call
/// ***
int scan::NetUtil::wsa_cleanup()
{
    int wsa_rc{ NO_ERROR };

    if (m_wsa_call_count > 0)
    {
        // Cleanup WinSock resources
        for (uint i{ m_wsa_call_count }; i > 0; i--)
        {
            wsa_rc = WSACleanup();
            m_wsa_call_count--;
        }
    }
    return wsa_rc;
}

/// ***
/// Handle WinSock required WSAStartup function call
/// ***
int scan::NetUtil::wsa_startup(const string &t_addr)
{
    int wsa_rc{ NO_ERROR };

    // Initialize use of Winsock DLL
    if (m_wsa_call_count == 0)
    {
        WSAData wsadata{ 0 };

        if ((wsa_rc = WSAStartup(SOCKV, &wsadata)) != NO_ERROR)
        {
            error(t_addr, wsa_rc);
        }
        m_wsa_call_count += 1;
    }
    return wsa_rc;
}

/// ***
/// Get a summary of the scan statistics as a string
/// ***
std::string scan::NetUtil::scan_summary(const string &t_target,
                                        const Timer &t_timer,
                                        const string &t_outpath) {
    std::stringstream ss;
    const string title{ "Scan Summary" };

    ss << title << stdu::LF
        << string(title.size(), '-') << stdu::LF
        << "Duration   : " << t_timer.elapsed_str() << stdu::LF
        << "Start Time : " << Timer::timestamp(t_timer.beg_time()) << stdu::LF
        << "End Time   : " << Timer::timestamp(t_timer.end_time());

    // Include output file path
    if (!t_outpath.empty())
    {
        ss << stdu::LF << "Output     : '" << t_outpath << "'";
    }
    return ss.str();
}

/// ***
/// Modify service information for the given service reference
/// ***
scan::SvcInfo scan::NetUtil::update_svc(SvcInfo &t_si, const HostState &t_hs)
{
    if (!valid_port(t_si.port))
    {
        throw ArgEx("t_si.port", "Invalid port number");
    }

    // Service already known
    if (!t_si.service.get().empty() && (t_si.service.get() != "unknown"))
    {
        return t_si;
    }

    // Invalid port number
    if (!valid_port(t_si.port))
    {
        throw ArgEx("t_si.port", "Port number must be between 0 and 65535");
    }

    load_info();
    const array_s fields{ m_svcvect[std::stoi(t_si.port.get())] };

    t_si.state = t_hs;
    t_si.proto = fields[1];
    t_si.service = fields[2];
    t_si.info = fields[3];

    return t_si;
}
