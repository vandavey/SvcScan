/*
*  socket.cpp
*  ----------
*  Source file for IPv4 TCP socket wrapper class
*/
#include <iostream>
#include <ws2tcpip.h>
#include "includes/container/svctable.h"
#include "includes/except/nullptrex.h"
#include "includes/net/endpoint.h"
#include "includes/net/socket.h"

#pragma comment(lib, "ws2_32.lib")

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
    m_addr = t_sock.m_addr;
    m_ports = t_sock.m_ports;
    m_services = t_sock.m_services;
    m_sock = t_sock.m_sock;

    addr = &m_addr;
    ports = &m_ports;
}

/// ***
/// Initialize the object
/// ***
scan::Socket::Socket(const string &t_addr, const list_s &t_ports)
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

    addr = &m_addr;
    ports = &m_ports;

    return *this;
}

/// ***
/// Determine if port string is a valid network port
/// ***
bool scan::Socket::valid_port(const string &t_port)
{
    for (const uchar &ch : t_port)
    {
        // Can't parse as integer
        if (!std::isdigit(ch))
        {
            return false;
        }
    }
    const int iport{ std::stoi(t_port) };
    return (iport >= 0) && (iport <= 65535);
}

/// ***
/// Determine if vector strings are valid network ports
/// ***
bool scan::Socket::valid_port(const vector_s &t_ports)
{
    for (const string &port : t_ports)
    {
        if (!valid_port(port))
        {
            return false;
        }
    }
    return true;
}

/// ***
/// Determine if IPv4 (dot-decimal notation) is valid
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
    return (code == 1) ? 0 : 1;
}

/// ***
/// Connect to the remote host
/// ***
void scan::Socket::connect()
{
    int wsa_rc;
    WSAData wsadata;

    // Initialize use of Winsock DLL
    if ((wsa_rc = WSAStartup(SOCKV, &wsadata)) != 0)
    {
        error(wsa_rc);
        m_sock = INVALID_SOCKET;
        return;
    }

    // Invalid network ports
    if (!valid_port(m_ports))
    {
        throw ArgEx("m_ports", "Invalid port number");
    }

    // Print scan start message
    Util::printf("Beginning scan against %", m_addr);

    // Connect to each port in underlying ports list
    for (const string &port : m_ports)
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
        int rc{ SOCKET_ERROR };

        // Put socket into non-blocking mode
        if ((rc = set_blocking(false)) != NO_ERROR)
        {
            error();
            close(ai_ptr);
            m_services.add(update_svc(si, port, HostState::unknown));
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

        // Parse TCP application banner
        if (buffstr.empty())
        {
            update_svc(si, port, hs);
        }
        m_services.add(si);

        // Close socket and free addrinfoW
        close(ai_ptr);
    }

    if (Parser::verbose)
    {
        std::cout << Util::LF;
    }
    std::cout << SvcTable(m_addr, m_services) << Util::LF;

    WSACleanup();
}

/// ***
/// Close underlying socket and set its handle to default
/// ***
void scan::Socket::close(addrinfoW *t_aiptr)
{
    if (!valid_sock(m_sock))
    {
        throw ArgEx("m_sock", "Invalid socket descriptor");
    }

    // Free dynamic resources
    if (t_aiptr != nullptr)
    {
        FreeAddrInfoW(t_aiptr);
    }

    // Attempt to close socket descriptor
    if (closesocket(m_sock) == SOCKET_ERROR)
    {
        error();
    }
    m_sock = INVALID_SOCKET;
}

/// ***
/// Print WSA error information to standard error
/// ***
void scan::Socket::error() const
{
    const int err{ get_error() };
    error((!err ? -1 : err), string());
}

/// ***
/// Print WSA error information to standard error
/// ***
void scan::Socket::error(const int &t_err) const
{
    if (t_err == NULL)
    {
        throw NullArgEx{ "t_err" };
    }
    error(t_err, string());
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void scan::Socket::error(const string &t_arg) const
{
    const int err{ get_error() };
    error((!err ? -1 : err), t_arg);
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void scan::Socket::error(const int &t_err, const string &t_arg) const
{
    if (t_err == NULL)
    {
        throw NullArgEx{ "t_err" };
    }
    const string dest{ t_arg.empty() ? "destination host" : t_arg };

    switch (t_err)
    {
        case WSAHOST_NOT_FOUND:  // Target host not found
            Util::errorf("Target host not found: %", dest);
            break;
        case WSAECONNREFUSED:    // Connection refused
            Util::errorf("Connection refused by %", dest);
            break;
        case WSAECONNRESET:      // Connection reset
            Util::errorf("Connection forcibly closed by %", dest);
            break;
        case WSAEHOSTDOWN:       // Destination host down
            Util::errorf("% is down or unresponsive", dest);
            break;
        case WSANOTINITIALISED:  // WSAStartup call missing
            Util::error("WSAStartup call missing");
            break;
        case WSAETIMEDOUT:       // Recv/send timeout
        case WSAEWOULDBLOCK:     // Connect timeout
            Util::errorf("Connection timeout: %", dest);
            break;
        default:                 // Default (error code)
            Util::errorf("Winsock error: %", Util::itos(t_err));
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
        throw ArgEx("m_sock", "Invalid socket descriptor");
    }

    if (t_aiptr == nullptr)
    {
        throw NullPtrEx{ "t_aiptr" };
    }
    fd_set fds{ 1, { m_sock } };

    int ec;
    const int addr_len{ static_cast<int>(t_aiptr->ai_addrlen) };

    // Connect to the remote host
    int rc{ ::connect(m_sock, t_aiptr->ai_addr, addr_len) };

    if (rc == SOCKET_ERROR)
    {
        // Connection attempt failed
        if ((ec = get_error()) != WSAEWOULDBLOCK)
        {
            if (Parser::verbose)
            {
                error(ec, t_ep);
            }
            return HostState::closed;
        }

        // Handle connection failures/timeouts
        if ((rc = select(nullptr, &fds, { 3, 500 })) != 1)
        {
            if (Parser::verbose)
            {
                error(t_ep);
            }
            return (rc == -1) ? HostState::closed : HostState::unknown;
        }
    }
    HostState hs{ HostState::unknown };

    // Print connection message
    if (Parser::verbose)
    {
        Util::print(Util::fmt("Connection established to %", t_ep));
    }

    // Connected - check stream readability
    switch (select(&fds, nullptr, { 1, 0 }))
    {
        case -1:  // Socket failure
        {
            if (Parser::verbose)
            {
                error();
            }
            hs = HostState::closed;
            break;
        }
        case 1:   // Banner available
        {
            if (::recv(m_sock, t_buffer, BUFFER_SIZE, 0) == SOCKET_ERROR)
            {
                error();
            }
            [[fallthrough]];
        }
        case 0:   // Banner not available
        {
            hs = HostState::open;
            break;
        }
        default:
        {
            break;
        }
    }

    shutdown(m_sock, SHUT_RD);
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
int scan::Socket::select(fd_set *t_read_fdsp, fd_set *t_write_fdsp,
                                              const timeval &t_to) const {
    if (!t_read_fdsp && !t_write_fdsp)
    {
        throw NullPtrEx{ "t_read_fdsp", "t_write_fdsp" };
    }

    // Determine if socket is readable/writable 
    int rc{ ::select(0, t_read_fdsp, t_write_fdsp, nullptr, &t_to) };

    // Return socket polling result
    if (rc != NO_ERROR)
    {
        return rc;
    }

    timeval ex_to{ 0, 1 };
    fd_set efds{ 1, m_sock };

    // Handle exception polling results
    switch (rc = ::select(0, nullptr, nullptr, &efds, &ex_to))
    {
        case SOCKET_ERROR:  // Error occurred
            return rc;
        case NO_ERROR:      // Timeout occurred
            WSASetLastError(static_cast<int>(WSAETIMEDOUT));
            return rc;
        default:
            break;
    }

    int err{ 0 };
    int optlen{ sizeof(int) };

    // Retrieve socket specific error
    rc = getsockopt(m_sock, SOL_SOCKET, SO_ERROR,
                                        reinterpret_cast<char *>(&err),
                                        &optlen);

    // Update WSA error with socket error
    if (rc == NO_ERROR)
    {
        WSASetLastError(err);
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
        throw ArgEx("m_sock", "Invalid socket descriptor");
    }
    ulong mode{ static_cast<ulong>(t_do_block ? 0 : 1) };

    // Modify socket blocking
    return ::ioctlsocket(m_sock, FIONBIO, &mode);
}

/// ***
/// Prepare socket for connection to destination host
/// ***
addrinfoW *scan::Socket::startup(SvcInfo &t_si, const string &t_port)
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
    rc = GetAddrInfoW(Util::wstr(m_addr).c_str(), Util::wstr(t_port).c_str(),
                                                  &ai_hints, 
                                                  &aiptr);
    // Handle DNS lookup errors
    if (rc != 0)
    {
        error(EndPoint(m_addr, t_port));
        FreeAddrInfoW(aiptr);
        m_sock = INVALID_SOCKET;

        m_services.add(update_svc(t_si, t_port, HostState::unknown));
        return nullptr;
    }
    m_sock = socket(aiptr->ai_family, aiptr->ai_socktype, aiptr->ai_protocol);

    // Handle socket startup failure
    if (!valid_sock(m_sock))
    {
        error();
        FreeAddrInfoW(aiptr);
        m_sock = INVALID_SOCKET;

        m_services.add(update_svc(t_si, t_port, HostState::unknown));
        aiptr = nullptr;
    }
    return aiptr;
}

/// ***
/// Modify service information for the given service reference
/// ***
scan::SvcInfo &scan::Socket::update_svc(SvcInfo &t_si,
                                        const string &t_port,
                                        const HostState &t_hs) const {
    // Invalid port number
    if (!valid_port(t_port))
    {
        throw ArgEx("t_port", "Invalid port number");
    }

    if (!t_si.banner.get().empty())
    {
        return t_si;
    }
    const char addr[]{ "0.0.0.0" };

    int code;
    int iaddr{ SOCKET_ERROR };

    // Convert IPv4 string to binary address
    if (inet_pton(AF_INET, &addr[0], &iaddr) != 1)
    {
        error();
        return t_si;
    }

    // Initialize sockaddr_in structure
    sockaddr_in sa{ AF_INET };
    sa.sin_addr.s_addr = iaddr;
    sa.sin_port = htons(static_cast<ushort>(std::stoi(t_port)));

    // Reinterpret sockaddr_in memory address as sockaddr pointer
    const sockaddr *sa_ptr{ reinterpret_cast<sockaddr *>(&sa) };

    char host_buffer[NI_MAXHOST]{ 0 };
    char svc_buffer[NI_MAXSERV]{ 0 };

    // Resolve service information
    code = getnameinfo(sa_ptr, sizeof(sa), host_buffer, NI_MAXHOST, svc_buffer,
                                                                    NI_MAXSERV,
                                                                    NULL);
    t_si.state = t_hs;
    t_si.service = code ? "" : svc_buffer;

    return t_si;
}
