/*
*  socket.cpp
*  ----------
*  Source file for IPv4 TCP socket wrapper class
*/
#include <algorithm>
#include <iostream>
#include <ws2tcpip.h>
#include "includes/containers/svctable.h"
#include "includes/except/nullptrex.h"
#include "includes/filesys/filestream.h"
#include "includes/inet/endpoint.h"
#include "includes/inet/socket.h"

#pragma comment(lib, "ws2_32.lib")

scan::AutoProp<std::string> scan::Socket::out_path{ string() };

timeval scan::Socket::m_timeout{ 3, 500 };

/// ***
/// Initialize the object
/// ***
scan::Socket::Socket()
{
    addr = &m_addr;
    ports = &m_ports;
}

/// ***
/// Initialize the object
/// ***
scan::Socket::Socket(const Socket &t_sock)
{
    operator=(t_sock);
}

/// ***
/// Initialize the object
/// ***
scan::Socket::Socket(const string &t_addr, const list_ui &t_ports)
{
    m_addr = t_addr;
    m_ports = t_ports;

    addr = &m_addr;
    ports = &m_ports;
}

/// ***
/// Destroy the object
/// ***
scan::Socket::~Socket()
{
    WSACleanup();
}

/// ***
/// Assignment operator overload
/// ***
scan::Socket &scan::Socket::operator=(const Socket &t_sock) noexcept
{
    m_addr = t_sock.m_addr;
    m_ports = t_sock.m_ports;
    m_services = t_sock.m_services;
    m_sock = t_sock.m_sock;
    m_timer = t_sock.m_timer;

    addr = &m_addr;
    ports = &m_ports;

    return *this;
}

/// ***
/// Set the socket timeout time value
/// ***
void scan::Socket::set_timeout(const uint &t_sec, const uint &t_ms)
{
    m_timeout = { static_cast<long>(t_sec), static_cast<long>(t_ms) };
}

/// ***
/// Determine if given integer is a valid network port
/// ***
bool scan::Socket::valid_port(const int &t_port)
{
    return (t_port >= 0) && (t_port <= 65535);
}

/// ***
/// Determine if port string is a valid network port
/// ***
bool scan::Socket::valid_port(const string &t_port)
{
    bool is_digit = std::all_of(t_port.begin(), t_port.end(), [](const char &l_port)
    {
        return std::isdigit(l_port);
    });

    // Can't parse as integer
    if (!is_digit)
    {
        return false;
    }
    return valid_port(std::stoi(t_port));
}

/// ***
/// Determine if vector strings are valid network ports
/// ***
bool scan::Socket::valid_port(const vector_ui &t_ports)
{
    return std::all_of(t_ports.begin(), t_ports.end(), [](const uint &l_port)
    {
        return valid_port(l_port);
    });
}

/// ***
/// Determine if IPv4 (dotted-quad notation) is valid
/// ***
int scan::Socket::valid_ip(const string &t_addr)
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
/// Connect to the remote host
/// ***
void scan::Socket::connect()
{
    WSAData wsadata{ 0 };

    // Initialize use of Winsock DLL
    if (int wsa_rc{ WSAStartup(SOCKV, &wsadata) }; wsa_rc != NO_ERROR)
    {
        error(m_addr, wsa_rc);
        m_sock = INVALID_SOCKET;
        return;
    }

    // Invalid network ports
    if (!valid_port(m_ports))
    {
        throw ArgEx("m_ports", "Invalid port number");
    }

    // Print scan start message
    stdu::printf("Beginning scan against %", m_addr);

    m_timer.start();

    /*
    *  TODO: Add 'scanning <target>... at <start_time>'
    */

    // Connect to each port in underlying ports list
    for (const int &port : m_ports)
    {
        m_sock = INVALID_SOCKET;
        const EndPoint ep(m_addr, port);

        SvcInfo si{ ep };
        addrinfoW *ai_ptr{ startup(si, port) };

        // Invalid socket descriptor
        if (!valid_sock(m_sock))
        {
            FreeAddrInfoW(ai_ptr);
            continue;
        }

        // Put socket into non-blocking mode
        if (set_blocking(false) != NO_ERROR)
        {
            error(ep);
            close(ai_ptr);
            m_services.add(update_svc(si, HostState::unknown));
            return;
        }
        char buffer[BUFFER_SIZE]{ 0 };

        // Connect to the remote endpoint specified
        const HostState hs{ connect(ai_ptr, buffer, ep) };
        const string buffstr{ buffer };

        if ((hs == HostState::open) && !buffstr.empty())
        {
            si.parse(buffer);
        }

        update_svc(si, hs);
        m_services.add(si);

        // Close socket and free addrinfoW
        close(ai_ptr);
    }

    m_timer.stop();
    const string summary{ scan_summary(m_addr, m_timer) };

    if (Parser::verbose)
    {
        std::cout << LF;
    }
    std::cout << summary << LF << LF << SvcTable(m_addr, m_services) << LF;

    // Write scan results to output file
    if (!out_path.get().empty())
    {
        FileStream fs{ out_path };
        fs << summary << LF << LF << SvcTable(m_addr, m_services) << LF;

        // Print output file path
        if (Parser::verbose)
        {
            stdu::printf("Scan report file: '%'\n", fs.path);
        }
        fs.close();
    }
    WSACleanup();
}

/// ***
/// Close underlying socket and set its handle to default
/// ***
void scan::Socket::close(addrinfoW *t_aiptr)
{
    if (!valid_sock(m_sock))
    {
        throw LogicEx("Socket::close", "Invalid underlying socket");
    }

    // Free dynamic resources
    if (t_aiptr != nullptr)
    {
        FreeAddrInfoW(t_aiptr);
    }

    // Attempt to close socket descriptor
    if (closesocket(m_sock) == SOCKET_ERROR)
    {
        error(m_addr);
    }
    m_sock = INVALID_SOCKET;
}

/// ***
/// Format and print WSA error message to standard error stream
/// ***
void scan::Socket::error(const string &t_addr) const
{
    error(EndPoint(t_addr));
}

/// ***
/// Format and print WSA error message to standard error stream
/// ***
void scan::Socket::error(const EndPoint &t_ep, const int &t_err) const
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
/// Determine if socket is valid
/// ***
bool scan::Socket::valid_sock(const SOCKET &t_sock) const noexcept
{
    if (t_sock == NULL)
    {
        return false;
    }
    return (t_sock != INVALID_SOCKET) && (t_sock != SOCKET_ERROR);
}

/// ***
/// Connect to the remote host and read banner data
/// ***
scan::HostState scan::Socket::connect(addrinfoW *t_aiptr,
                                      char (&t_buffer)[BUFFER_SIZE],
                                      const EndPoint &t_ep) {
    if (!valid_sock(m_sock))
    {
        throw LogicEx("Socket::connect", "Invalid underlying socket");
    }

    if (t_aiptr == nullptr)
    {
        throw NullPtrEx{ "t_aiptr" };
    }

    const int addr_len{ static_cast<int>(t_aiptr->ai_addrlen) };

    // Connect to the remote host
    int rc{ ::connect(m_sock, t_aiptr->ai_addr, addr_len) };

    if (rc == SOCKET_ERROR)
    {
        // Connection attempt failed
        if (int ec{ get_error() }; ec != WSAEWOULDBLOCK)
        {
            if (Parser::verbose)
            {
                error(t_ep, ec);
            }
            return HostState::closed;
        }
        fd_set fds{ 1, { m_sock } };

        // Handle connection failures/timeouts
        if (rc = select(nullptr, &fds, m_timeout); rc != SOCKET_READY)
        {
            if (Parser::verbose)
            {
                error(t_ep);
            }
            return (rc == -1) ? HostState::closed : HostState::unknown;
        }
    }

    // Print connection message
    if (Parser::verbose)
    {
        stdu::printf("Connection established: %/tcp", t_ep.port);
    }

    // Read inbound socket data
    const HostState hs{ recv(t_buffer) };

    shutdown(m_sock, SHUT_RD);
    return hs;
}

/// ***
/// Read inbound socket data stream
/// ***
scan::HostState scan::Socket::recv(char (&t_buffer)[BUFFER_SIZE])
{
    if (t_buffer == NULL)
    {
        throw NullArgEx{ "t_buffer" };
    }

    if (!valid_sock(m_sock))
    {
        throw LogicEx("Socket::recv", "Invalid underlying socket");
    }

    fd_set fds{ 1, { m_sock } };
    HostState hs{ HostState::unknown };

    // Poll connected socket for readability
    switch (select(&fds, nullptr, timeval{ 1, 0 }))
    {
        case SOCKET_ERROR:  // Socket failure
        {
            if (Parser::verbose)
            {
                error(m_addr);
            }
            hs = HostState::closed;
            break;
        }
        case NO_ERROR:      // Unreadable stream
        {
            hs = HostState::open;
            break;
        }
        case SOCKET_READY:  // Readable stream
        {
            // Wait for inbound data
            Sleep(200);

            // Read inbound socket data
            if (::recv(m_sock, t_buffer, BUFFER_SIZE, 0) == SOCKET_ERROR)
            {
                error(m_addr);
            }
            hs = HostState::open;
            break;
        }
        default:
        {
            break;
        }
    }
    return hs;
}

/// ***
/// Get the last error using WSAGetLastError
/// ***
int scan::Socket::get_error() const
{
    return WSAGetLastError();
}

/// ***
/// Poll underlying socket for reading and writing
/// ***
int scan::Socket::select(fd_set *t_read_fdsp,
                         fd_set *t_write_fdsp,
                         const timeval &t_to) const {
    // Missing pointers
    if (!t_read_fdsp && !t_write_fdsp)
    {
        throw NullPtrEx({ "t_read_fdsp", "t_write_fdsp" });
    }

    // Determine if socket is readable/writable
    int rc{ ::select(0, t_read_fdsp, t_write_fdsp, nullptr, &t_to) };

    // Return socket polling result
    if (rc != NO_ERROR)
    {
        return rc;
    }

    timeval ex_to{ 0, 1 };
    fd_set ex_fds{ 1, m_sock };

    // Handle exception polling results
    switch (rc = ::select(0, nullptr, nullptr, &ex_fds, &ex_to))
    {
        case SOCKET_ERROR:  // Error occurred
            return rc;
        case NO_ERROR:      // Timeout occurred
            WSASetLastError(static_cast<int>(WSAETIMEDOUT));
            return rc;
        default:
            break;
    }

    int ec{ 0 };
    int optlen{ static_cast<int>(sizeof(int)) };

    // Retrieve socket specific error
    rc = getsockopt(m_sock,
                    SOL_SOCKET,
                    SO_ERROR,
                    reinterpret_cast<char *>(&ec),
                    &optlen);

    // Update WSA error with socket error
    if (rc == NO_ERROR)
    {
        WSASetLastError(ec);
        rc = SOCKET_ERROR;
    }
    return rc;
}

/// ***
/// Configure blocking options on underlying socket
/// ***
int scan::Socket::set_blocking(const bool &t_do_block)
{
    if (!valid_sock(m_sock))
    {
        throw LogicEx("Socket::set_blocking", "Invalid underlying socket");
    }
    ulong mode{ static_cast<ulong>(t_do_block ? 0 : 1) };

    // Modify socket blocking
    return ::ioctlsocket(m_sock, FIONBIO, &mode);
}

/// ***
/// Prepare socket for connection to destination host
/// ***
addrinfoW *scan::Socket::startup(SvcInfo &t_si, const uint &t_port)
{
    if (m_sock == NULL)
    {
        throw NullArgEx{ "m_sock" };
    }

    if (!valid_port(t_port))
    {
        throw ArgEx("t_port", "Invalid port number");
    }
    int rc;

    addrinfoW *aiptr{ nullptr };
    addrinfoW ai_hints{ AI_CANONNAME, AF_INET, SOCK_STREAM, IPPROTO_TCP };

    // Avoid WSAHOST_NOT_FOUND false positive
    Sleep(500);

    // Resolve address information
    rc = GetAddrInfoW(Util::wstr(m_addr).c_str(),
                      std::to_wstring(t_port).c_str(),
                      &ai_hints,
                      &aiptr);

    // Handle DNS lookup errors
    if (rc != NO_ERROR)
    {
        error(EndPoint(m_addr, t_port));
        FreeAddrInfoW(aiptr);

        m_sock = INVALID_SOCKET;
        m_services.add(update_svc(t_si, HostState::unknown));

        return nullptr;
    }
    m_sock = socket(aiptr->ai_family, aiptr->ai_socktype, aiptr->ai_protocol);

    // Handle socket startup failure
    if (!valid_sock(m_sock))
    {
        error(EndPoint(m_addr, t_port));
        FreeAddrInfoW(aiptr);
        m_sock = INVALID_SOCKET;

        m_services.add(update_svc(t_si, HostState::unknown));
        aiptr = nullptr;
    }
    return aiptr;
}

/// ***
/// Get scan a summary of the scan statistics
/// ***
std::string scan::Socket::scan_summary(const string &t_target,
                                       const Timer &t_timer,
                                       const string &t_title) const {
    std::stringstream ss;

    ss << t_title << LF
        << string(t_title.size(), '-') << LF
        << "Duration : " << t_timer.elapsed_str() << LF
        << "Started  : " << Timer::timestamp(t_timer.beg_time()) << LF
        << "Ended    : " << Timer::timestamp(t_timer.end_time());

    return ss.str();
}

/// ***
/// Modify service information for the given service reference
/// ***
scan::SvcInfo &scan::Socket::update_svc(SvcInfo &t_si, const HostState &t_hs) const
{
    // Invalid port number
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
        error(m_addr);
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
