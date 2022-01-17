/*
*  netutil.cpp
*  -----------
*  Source file for network and socket utilities
*/
#include <algorithm>
#include <ws2tcpip.h>
#include "includes/except/nullargex.h"
#include "includes/filesys/filestream.h"
#include "includes/inet/netutil.h"

#pragma comment(lib, "ws2_32.lib")

scan::NetUtil::uint scan::NetUtil::m_wsa_call_count{ 0 };

scan::TextRc scan::NetUtil::m_csv_rc{ CSV_DATA };

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
            stdu::errorf("WinSock error: %", err);
            break;
    }
}

/// ***
/// Determine whether the IPv4 string (dotted-quad notation) is valid
/// ***
bool scan::NetUtil::valid_ipv4(const string &t_addr)
{
    bool valid_ip{ false };

    if (valid_ipv4_fmt(t_addr))
    {
        int iaddr{ SOCKET_ERROR };
        const int code{ inet_pton(AF_INET, t_addr.c_str(), &iaddr) };

        valid_ip = code == SOCKET_READY;
    }
    return valid_ip;
}

/// ***
/// Determine whether the IPv4 string (dotted-quad notation) is in a valid format
/// ***
bool scan::NetUtil::valid_ipv4_fmt(const string &t_addr)
{
    bool valid_fmt{ false };

    if (Util::count(t_addr, '.') == 3)
    {
        for (const string &octet : Util::split(t_addr, "."))
        {
            if (!(valid_fmt = Util::is_integral(octet)))
            {
                break;
            }
        }
    }
    return valid_fmt;
}

/// ***
/// Determine whether the given integer is a valid network port
/// ***
bool scan::NetUtil::valid_port(const int &t_port)
{
    return (t_port >= 0) && (t_port <= MAX_PORT);
}

/// ***
/// Determine whether the given string is a valid network port
/// ***
bool scan::NetUtil::valid_port(const string &t_port)
{
    return !t_port.empty()
        && Util::is_integral(t_port)
        && valid_port(std::stoi(t_port));
}

/// ***
/// Determine whether the vector integers are valid network ports
/// ***
bool scan::NetUtil::valid_port(const vector_ui &t_ports)
{
    return std::all_of(t_ports.cbegin(), t_ports.cend(), [](const int &l_port)
    {
        return valid_port(l_port);
    });
}

/// ***
/// Determine whether the given socket is valid
/// ***
bool scan::NetUtil::valid_sock(const SOCKET &t_sock) noexcept
{
    return t_sock && (t_sock != INVALID_SOCKET) && (t_sock != SOCKET_ERROR);
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
        throw ArgEx{ "t_sock", "The given socket is invalid" };
    }
    ulong mode{ static_cast<ulong>(t_do_block ? 0 : 1) };

    // Modify socket blocking
    return ::ioctlsocket(t_sock, FIONBIO, &mode);
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

    // Initialize use of WinSock DLL
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
std::string scan::NetUtil::scan_progress(const uint &t_next_port,
                                         const list_ui &t_ports,
                                         const size_t &t_start_pos) {
    if (t_next_port == NULL)
    {
        throw NullArgEx{ "t_next_port" };
    }

    if (t_ports.empty())
    {
        throw ArgEx{ "t_ports", "Ports list cannot be empty" };
    }

    const size_t position{ t_ports.find(t_next_port, t_start_pos) };
    const size_t rem_num{ t_ports.size() - position };

    const double done_num{ static_cast<double>(position) };
    const double progress{ done_num / static_cast<double>(t_ports.size()) * 100 };

    sstream ss;
    const string rem_str{ (rem_num == 1) ? " port remaining" : " ports remaining" };

    // Set decimal point precision
    ss.precision(4);

    ss << "Scan " << progress << "% completed (" << rem_num << rem_str << ")";

    return ss.str();
}

/// ***
/// Get a summary of the scan statistics as a string
/// ***
std::string scan::NetUtil::scan_summary(const string &t_target,
                                        const Timer &t_timer,
                                        const string &t_outpath) {
    sstream ss;
    const string title{ "Scan Summary" };

    ss << title << stdu::LF
        << string(title.size(), '-') << stdu::LF
        << "Duration   : " << t_timer.elapsed_str() << stdu::LF
        << "Start Time : " << Timer::timestamp(t_timer.beg_time()) << stdu::LF
        << "End Time   : " << Timer::timestamp(t_timer.end_time());

    // Include output file path
    if (!t_outpath.empty())
    {
        ss << stdu::LF << Util::fstr("Report     : '%'", t_outpath);
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
        throw ArgEx{ "t_si.port", "Invalid port number" };
    }

    t_si.state = t_hs;
    const bool skip_info{ !t_si.info.empty() && (t_si.service == "unknown") };

    // Only resolve unknowns services
    if (t_si.service.empty() || skip_info)
    {
        // Invalid port number
        if (!valid_port(t_si.port))
        {
            throw ArgEx{ "t_si.port", "Port number must be between 0 and 65535" };
        }
        string csv_line;

        // Get the line from the CSV data
        if (m_csv_rc.get_line(csv_line, std::stoi(t_si.port)))
        {
            const array_s fields{ parse_fields(csv_line) };

            t_si.proto = fields[1];
            t_si.service = fields[2];

            // Update service information
            if (!skip_info)
            {
                t_si.info = fields[3];
            }
        }
    }
    return t_si;
}

/// ***
/// Parse the string fields from the given CSV record string
/// ***
scan::NetUtil::array_s scan::NetUtil::parse_fields(const string &t_csv_line)
{
    const string new_line{ Util::replace(t_csv_line, "\"", "") };
    const vector_s field_vect{ Util::split(new_line, ",", 3) };

    array_s fields;

    // Copy elements into array
    for (size_t i{ 0 }; i < field_vect.size(); i++)
    {
        if (i == fields.size())
        {
            break;
        }
        fields[i] = field_vect[i];
    }
    return fields;
}
