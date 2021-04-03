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
Scan::Socket::Socket()
{
    this->addr = &m_addr;
    this->ports = &m_ports;
}

/// ***
/// Initialize the object
/// ***
Scan::Socket::Socket(const Socket &sock)
{
    this->m_addr = sock.m_addr;
    this->m_ports = sock.m_ports;
    this->m_services = sock.m_services;
    this->m_sock = sock.m_sock;

    this->addr = &m_addr;
    this->ports = &m_ports;
}

/// ***
/// Initialize the object
/// ***
Scan::Socket::Socket(const property_s &addr, const property_l &ports)
{
    this->m_addr = addr.get();
    this->m_ports = ports.get();

    this->addr = &m_addr;
    this->ports = &m_ports;
}

/// ***
/// Destroy the object
/// ***
Scan::Socket::~Socket()
{
    WSACleanup();
}

/// ***
/// Assignment operator overload
/// ***
Scan::Socket &Scan::Socket::operator=(const Socket &sock) noexcept
{
    m_addr = sock.m_addr;
    m_ports = sock.m_ports;
    m_services = sock.m_services;
    m_sock = sock.m_sock;

    addr = &m_addr;
    ports = &m_ports;

    return *this;
}

/// ***
/// Determine if port string is a valid network port
/// ***
const bool Scan::Socket::valid_port(const string &port)
{
    for (const uchar &ch : port)
    {
        // Can't parse as integer
        if (!std::isdigit(ch))
        {
            return false;
        }
    }
    const int iport = {stoi(port)};
    return (iport >= 0) && (iport <= 65535);
}

/// ***
/// Determine if vector strings are valid network ports
/// ***
const bool Scan::Socket::valid_port(const vector_s &ports)
{
    for (const string &port : ports)
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
const int Scan::Socket::valid_ip(const string &addr)
{
    // Don't attempt resolution (invalid/unknown format)
    if (Util::count(addr, '.') != 3)
    {
        return SOCKET_ERROR;
    }
    int iaddr = {static_cast<int>(sizeof(in_addr))};

    // Convert IPv4 string to binary
    const int code = {inet_pton(AF_INET, addr.c_str(), &iaddr)};
    return (code == 1) ? 0 : 1;
}

/// ***
/// Connect to the remote host
/// ***
void Scan::Socket::connect()
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
        throw ArgEx("port", "Invalid port number");
    }

    // Print scan start message
    Util::printf("Beginning scan against %", m_addr);

    // Connect to each port in underlying ports list
    for (const string &port : m_ports)
    {
        m_sock = INVALID_SOCKET;
        const EndPoint ep(m_addr, port);

        SvcInfo si(ep);
        addrinfoW *ai_ptr = {startup(si, port)};

        // Invalid socket descriptor
        if (!valid_sock(m_sock))
        {
            FreeAddrInfoW(ai_ptr);
            continue;
        }
        int rc = {SOCKET_ERROR};

        // Put socket into non-blocking mode
        if ((rc = set_blocking(false)) != NO_ERROR)
        {
            error();
            close(ai_ptr);
            m_services += update_svc(si, port, HostState::unknown);
            return;
        }
        char buffer[BUFFER_SIZE] = {0};

        // Connect to the remote endpoint specified
        const HostState hs = {connect(ai_ptr, buffer, ep)};
        const string buffstr(buffer);

        if ((hs == HostState::open) && !buffstr.empty())
        {
            si.parse(buffer);
        }

        // Parse TCP application banner
        if (buffstr.empty())
        {
            update_svc(si, port, hs);
        }
        m_services += si;

        // Close socket and free addrinfoW
        close(ai_ptr);
    }

    if (Parser::verbose.get())
    {
        std::cout << Util::LF;
    }

    std::cout << SvcTable(m_addr, m_services) << Util::LF;
    WSACleanup();
}

/// ***
/// Close underlying socket and set its handle to default
/// ***
void Scan::Socket::close(addrinfoW *ai_ptr)
{
    if (!valid_sock(m_sock))
    {
        throw ArgEx("m_sock", "Invalid socket descriptor");
    }

    // Free dynamic resources
    if (ai_ptr != nullptr)
    {
        FreeAddrInfoW(ai_ptr);
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
void Scan::Socket::error() const
{
    const int err = {get_error()};
    error((!err ? -1 : err), string());
}

/// ***
/// Print WSA error information to standard error
/// ***
void Scan::Socket::error(const int &err) const
{
    if (err == NULL)
    {
        throw NullArgEx("err");
    }
    error(err, string());
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void Scan::Socket::error(const string &arg) const
{
    const int err = {get_error()};
    error((!err ? -1 : err), arg);
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void Scan::Socket::error(const int &err, const string &arg) const
{
    if (err == NULL)
    {
        throw NullArgEx("err");
    }
    const string dest(arg.empty() ? "destination host" : arg);

    switch (err)
    {
        case WSAETIMEDOUT:       // Recv/send timeout
        case WSAEWOULDBLOCK:     // Connect timeout
        {
            Util::errorf("Connection timeout: %", dest);
            break;
        }
        case WSAHOST_NOT_FOUND:  // Target host not found
        {
            Util::errorf("Target host not found: %", dest);
            break;
        }
        case WSAECONNREFUSED:    // Connection refused
        {
            Util::errorf("Connection refused by %", dest);
            break;
        }
        case WSAECONNRESET:      // Connection reset
        {
            Util::errorf("Connection forcibly closed by %", dest);
            break;
        }
        case WSAEHOSTDOWN:       // Destination host down
        {
            Util::errorf("% is down or unresponsive", dest);
            break;
        }
        case WSANOTINITIALISED:  // WSAStartup call missing
        {
            Util::error("WSAStartup call missing");
            break;
        }
        default:                 // Default (error code)
        {
            Util::errorf("Winsock error: %", Util::itos(err));
            break;
        }
    }
}

/// ***
/// Determine if socket is valid
/// ***
const bool Scan::Socket::valid_sock(const SOCKET &sock) const noexcept
{
    if (sock == NULL)
    {
        return false;
    }
    return (sock != INVALID_SOCKET) && (sock != SOCKET_ERROR);
}

/// ***
/// Connect to the remote host and read banner data
/// ***
const Scan::HostState Scan::Socket::connect(addrinfoW *ai_ptr,
                                            char (&buffer)[BUFFER_SIZE],
                                            const EndPoint &ep) {
    if (!valid_sock(m_sock))
    {
        throw ArgEx("m_sock", "Invalid socket descriptor");
    }

    if (ai_ptr == nullptr)
    {
        throw NullPtrEx("ai_ptr");
    }
    fd_set fds = {1, {m_sock}};

    int ec = {SOCKET_ERROR};
    const int addr_len = {static_cast<int>(ai_ptr->ai_addrlen)};

    // Connect to the remote host
    int rc = ::connect(m_sock, ai_ptr->ai_addr, addr_len);

    if (rc == SOCKET_ERROR)
    {
        // Connection attempt failed
        if ((ec = get_error()) != WSAEWOULDBLOCK)
        {
            if (Parser::verbose.get())
            {
                error(ec, ep);
            }
            return HostState::closed;
        }

        // Handle connection failures/timeouts
        if ((rc = select(nullptr, &fds, {3, 500})) != 1)
        {
            if (Parser::verbose.get())
            {
                error(ep);
            }
            return (rc == -1) ? HostState::closed : HostState::unknown;
        }
    }
    HostState hs = {HostState::unknown};

    // Print connection message
    if (Parser::verbose.get())
    {
        Util::print(Util::fmt("Connection established to %", ep));
    }

    // Connected - check stream readability
    switch (select(&fds, nullptr, {1, 0}))
    {
        case -1:  // Socket failure
        {
            if (Parser::verbose.get())
            {
                error();
            }
            hs = HostState::closed;
            break;
        }
        case 1:   // Banner available
        {
            if (::recv(m_sock, buffer, BUFFER_SIZE, 0) == SOCKET_ERROR)
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
const int Scan::Socket::get_error() const
{
    return WSAGetLastError();
}

/// ***
/// Poll underlying socket for reading and writing
/// ***
const int Scan::Socket::select(fd_set *rfds_ptr, fd_set *wfds_ptr,
                                                 const timeval &to) const {
    if (!rfds_ptr && !wfds_ptr)
    {
        throw NullPtrEx({"wfds_ptr", "efds_ptr"});
    }

    // Determine if socket is readable/writable 
    int rc = {::select(0, rfds_ptr, wfds_ptr, nullptr, &to)};

    // Return socket polling result
    if (rc != NO_ERROR)
    {
        return rc;
    }

    timeval ex_to = {0, 1};
    fd_set efds = {1, m_sock};

    // Handle exception polling results
    switch (rc = ::select(0, nullptr, nullptr, &efds, &ex_to))
    {
        case SOCKET_ERROR:
            return rc;
        case NO_ERROR:
            WSASetLastError(static_cast<int>(WSAETIMEDOUT));
            return rc;
        default:
            break;
    }

    int err = {0};
    int optlen = {sizeof(int)};

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
const int Scan::Socket::set_blocking(const bool &do_block)
{
    if (!valid_sock(m_sock))
    {
        throw ArgEx("m_sock", "Invalid socket descriptor");
    }
    ulong mode = {static_cast<ulong>(do_block ? 0 : 1)};

    // Modify socket blocking
    return ::ioctlsocket(m_sock, FIONBIO, static_cast<ulong *>(&mode));
}

/// ***
/// Prepare socket for connection to destination host
/// ***
addrinfoW *Scan::Socket::startup(SvcInfo &si, const string &port)
{
    if (m_sock == NULL)
    {
        throw NullArgEx("m_sock");
    }

    if (!valid_port(port))
    {
        throw ArgEx("port", "Invalid port number");
    }
    int rc;

    addrinfoW *ptr = {nullptr};
    addrinfoW ai_hints = {AI_CANONNAME, AF_INET, SOCK_STREAM, IPPROTO_TCP};

    // Avoid WSAHOST_NOT_FOUND false positive
    Sleep(500);

    // Resolve address information
    rc = GetAddrInfoW(Util::utf16(m_addr).c_str(),
                      Util::utf16(port).c_str(),
                      static_cast<addrinfoW *>(&ai_hints),
                      static_cast<addrinfoW **>(&ptr));

    // Handle DNS lookup errors
    if (rc != 0)
    {
        error(EndPoint(m_addr, port));
        FreeAddrInfoW(ptr);
        m_sock = INVALID_SOCKET;

        m_services += update_svc(si, port, HostState::unknown);
        return nullptr;
    }
    m_sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    // Handle socket startup failure
    if (!valid_sock(m_sock))
    {
        error();
        FreeAddrInfoW(ptr);
        m_sock = INVALID_SOCKET;

        m_services += update_svc(si, port, HostState::unknown);
        ptr = nullptr;
    }
    return ptr;
}

/// ***
/// Modify service information for the given service reference
/// ***
Scan::SvcInfo &Scan::Socket::update_svc(SvcInfo &si,
                                        const string &port,
                                        const HostState &hs) const {
    // Invalid port number
    if (!valid_port(port))
    {
        throw ArgEx("port", "Invalid port number");
    }

    if (!si.banner.get().empty())
    {
        return si;
    }

    int code, iaddr = {SOCKET_ERROR};
    const char addr[] = {"0.0.0.0"};

    // Convert IPv4 string to binary address
    if (inet_pton(AF_INET, &addr[0], &iaddr) != 1)
    {
        error();
        return si;
    }

    // Initialize sockaddr_in structure
    sockaddr_in sa = {AF_INET};
    sa.sin_addr.s_addr = iaddr;
    sa.sin_port = htons(static_cast<ushort>(stoi(port)));

    // Reinterpret sockaddr_in reference as sockaddr pointer
    const sockaddr *sa_ptr(reinterpret_cast<sockaddr *>(&sa));

    char host_buffer[NI_MAXHOST] = {0};
    char svc_buffer[NI_MAXSERV] = {0};

    // Resolve service information
    code = getnameinfo(sa_ptr, sizeof(sa), host_buffer, NI_MAXHOST, svc_buffer,
                                                                    NI_MAXSERV,
                                                                    NULL);
    si.state = hs;
    si.service = code ? "" : svc_buffer;
    return si;
}
