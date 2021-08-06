/*
*  netutil.cpp
*  -----------
*  Source file for network and socket utilities
*/
#include <algorithm>
#include <ws2tcpip.h>
#include "includes/inet/netutil.h"

#pragma comment(lib, "ws2_32.lib")

scan::NetUtil::uint scan::NetUtil::m_wsa_call_count{ 0 };

/// ***
/// Destroy the object
/// ***
scan::NetUtil::~NetUtil()
{
    for (uint i{ 0 }; i < m_wsa_call_count; i++)
    {
        WSACleanup();
    }
}

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
/// Determine if the given integer is a valid network port
/// ***
bool scan::NetUtil::valid_port(const int &t_port)
{
    return (t_port >= 0) && (t_port <= 65535);
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
        wsa_rc = WSACleanup();
        m_wsa_call_count -= 1;
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

    int rc;
    int iaddr{ SOCKET_ERROR };

    // Convert IPv4 string to binary address
    if (inet_pton(AF_INET, &IPV4_ANY[0], &iaddr) != SOCKET_READY)
    {
        error(t_si.addr);
        return t_si;
    }

    // Initialize sockaddr_in structure
    sockaddr_in sa{ AF_INET };
    sa.sin_addr.s_addr = iaddr;
    sa.sin_port = htons(static_cast<ushort>(std::stoi(t_si.port)));

    // Reinterpret sockaddr_in pointer as sockaddr pointer
    const sockaddr *sa_ptr{ reinterpret_cast<sockaddr *>(&sa) };

    char host_buffer[NI_MAXHOST]{ 0 };
    char svc_buffer[NI_MAXSERV]{ 0 };

    // Resolve service information
    rc = getnameinfo(sa_ptr,
                     sizeof(sa),
                     host_buffer,
                     NI_MAXHOST,
                     svc_buffer,
                     NI_MAXSERV,
                     NULL);

    t_si.state = t_hs;
    t_si.service = (rc == NO_ERROR) ? t_si.service.get() : "unknown";

    // Update service information
    if (rc == NO_ERROR)
    {
        const string port{ t_si.port };
        const string port_sub{ port.substr(0, port.find("/tcp")) };

        // Update service property value
        t_si.service = (port_sub == svc_buffer) ? "unknown" : svc_buffer;
    }

    return t_si;
}
