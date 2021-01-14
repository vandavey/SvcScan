/*
*  socket.cpp
*  ----------
*  Source file for IPv4 TCP socket wrapper class
*/
#include <iostream>
#include <ws2tcpip.h>
#include "includes/except/nullargex.h"
#include "includes/net/socket.h"

#pragma comment(lib, "Ws2_32.lib")

namespace Scan
{
    using std::string;
}

/// ***
/// Initialize the object
/// ***
Scan::Socket::Socket()
{
}

/// ***
/// Initialize the object
/// ***
Scan::Socket::Socket(const Socket &sock)
{
    this->operator=(sock);
}

/// ***
/// Initialize the object
/// ***
Scan::Socket::Socket(const Property<string> &addr,
                     const Property<vector_s> &ports) {
    this->swap(addr, ports);
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
Scan::Socket &Scan::Socket::operator=(const Socket &sock)
{
    swap(sock);
    return *this;
}

/// ***
/// Determine if IPv4 (dot-decimal notation) is valid
/// ***
const int Scan::Socket::valid_ip(const string &addr)
{
    size_t i, next = {0};
    int count = {0}, code = {SOCKET_ERROR};

    while ((i = addr.find_first_not_of('.', next)) != -1)
    {
        if ((next = addr.find('.', i)) != -1)
        {
            count += 1;
        }
    }

    // Attempt address resolution
    if (count == 3)
    {
        int len = {static_cast<int>(sizeof(in_addr))};

        code = InetPtonW(AF_INET, Util::utf16(addr).c_str(),
                                  static_cast<int *>(&len));

        code = (code == 1) ? 0 : 1;
    }
    return code;
}

/// ***
/// Determine if port string is a valid network port
/// ***
const bool Scan::Socket::valid_port(const string &port)
{
    // Ensure port can be parsed as integer
    for (const uchar &ch : port)
    {
        if (!std::isdigit(ch))
        {
            return false;
        }
    }
    const int iport = {std::stoi(port)};
    return ((iport >= 0) && (iport <= 65535));
}

/// ***
/// Connect to the remote host
/// ***
void Scan::Socket::connect()
{
    for (const string &port : m_ports)
    {
        // Check for valid network port
        if (!valid_port(port))
        {
            throw ArgEx("port", "Invalid port number");
        }
        int code = {SOCKET_ERROR};

        SOCKET sock = {INVALID_SOCKET};
        addrinfoW *ptr = {startup(sock, port)};

        // Invalid socket descriptor
        if (!valid_sock(sock))
        {
            FreeAddrInfoW(ptr);
            continue;
        }
        const int sock_opts[] = {SO_RCVTIMEO, SO_SNDTIMEO};

        // Set socket options
        if ((code = setsockopts(sock, sock_opts)) != NO_ERROR)
        {
            error();
            FreeAddrInfoW(ptr);
            continue;
        }

        fd_set fds = {1, {sock}};
        EndPoint ep(m_addr, port);

        // Put socket into non-blocking mode
        if ((code = ioctl(sock, true)) != NO_ERROR)
        {
            error(ep.str());
            FreeAddrInfoW(ptr);
            continue;
        }
        code = ::connect(sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));

        if (code == SOCKET_ERROR)
        {
            // Connection failed
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                error(ep.str());
                FreeAddrInfoW(ptr);
                close(sock);
                continue;
            }

            // Connection timeout or failure
            if (select(nullptr, &fds) <= 0)
            {
                error(static_cast<int>(WSAEWOULDBLOCK), ep.str());
                FreeAddrInfoW(ptr);
                close(sock);
                continue;
            }
        }
        char buffer[BUFFERSIZE] = {0};

        // Check socket stream readability
        switch (select(&fds, nullptr))
        {
            /***
            * TODO: include domain name and addr in SvcInfo
            ***/
            case 1:             // Socket is readable
            {
                if ((code = recv(sock, buffer, BUFFERSIZE, 0)) > 0)
                {
                    SvcInfo si(ep, string(buffer, code));
                    std::cout << si << std::endl;
                    m_services.push_back(si);
                }
                break;
            }
            case NO_ERROR:      // Connection timeout
            {
                Util::warnf("No data received from %\n", ep.str());
                break;
            }
            case SOCKET_ERROR:  // Connection failure
            {
                error();
                break;
            }
        }
        shutdown(sock, SHUT_RDWR);
        close(sock);
        FreeAddrInfoW(ptr);
    }
}

/// ***
/// Close socket and set its handle to default value
/// ***
void Scan::Socket::close(SOCKET &sock) const
{
    if (closesocket(sock) == SOCKET_ERROR)
    {
        error();
    }
    sock = INVALID_SOCKET;
}

/// ***
/// Print WSA error information to standard error
/// ***
void Scan::Socket::error() const
{
    string arg;
    int err = {WSAGetLastError()};
    error((err ? err : -1), arg);
}

/// ***
/// Print WSA error information to standard error
/// ***
void Scan::Socket::error(const int &err) const
{
    if (err == NULL)
    {
        throw NullArgEx({"err"});
    }
    string arg;
    error(err, arg);
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void Scan::Socket::error(const string &arg) const
{
    int err = {WSAGetLastError()};
    error((err ? err : -1), const_cast<string &>(arg));
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void Scan::Socket::error(const int &err, string &arg) const
{
    if (err == NULL)
    {
        throw NullArgEx({"err"});
    }
    arg = arg.empty() ? "destination host" : arg;

    switch (err)
    {
        case WSAENSLOOKUP:       // DNS lookup error
        {
            Util::errorf("Can't resolve target: %\n", arg);
            break;
        }
        case WSAEWOULDBLOCK:     // Connect timeout
        {
            Util::errorf("Can't connect to %\n", arg);
            break;
        }
        case WSAETIMEDOUT:       // Recv/send timeout
        {
            Util::errorf("Connection timed out: %\n", arg);
            break;
        }
        case WSAECONNREFUSED:    // Connection refused
        {
            Util::errorf("Connection was refused by %\n", arg);
            break;
        }
        case WSAEHOSTDOWN:       // Destination host down
        {
            Util::errorf("% is down or unresponsive\n", arg);
            break;
        }
        case WSANOTINITIALISED:  // WSAStartup call missing
        {
            Util::error("Missing call to WSAStartup\n");
            break;
        }
        default:                 // Default (error code)
        {
            Util::errorf("Winsock error: %\n", Util::itos(err));
            break;
        }
    }
}

/// ***
/// Determine if socket is valid
/// ***
const bool Scan::Socket::valid_sock(const SOCKET &sock) const noexcept
{
    if ((sock == INVALID_SOCKET) || (sock == SOCKET_ERROR))
    {
        return false;
    }
    return true;
}

/// ***
/// Set options on the socket descriptor
/// ***
template<size_t N>
const int Scan::Socket::setsockopts(SOCKET &sock, const int (&opts)[N]) const
{
    if (!valid_sock(sock))
    {
        throw ArgEx("sock", "Invalid socket descriptor");
    }

    int code = {SOCKET_ERROR};
    const char *ptr = {Util::itoc(3500)};

    if (ptr == nullptr)
    {
        return code;
    }
    const int len = {static_cast<int>(sizeof(llong))};

    // Set socket options
    for (const int &opt : opts)
    {
        if (opt == NULL)
        {
            throw NullArgEx({"opt"});
        }
        code = ::setsockopt(sock, SOL_SOCKET, opt, ptr, len);

        // Failed to set socket option
        if (code != static_cast<int>(NO_ERROR))
        {
            Util::errorf("Failed to set sockopt '%'", Util::itos(opt));
        }
    }
    return code;
}

/// ***
/// Configure blocking options for the socket
/// ***
const int Scan::Socket::ioctl(SOCKET &sock, const bool &block) const
{
    if (!valid_sock(sock))
    {
        throw ArgEx("sock", "Invalid socket descriptor");
    }
    ulong arg = block ? 1 : 0;

    // Modify socket blocking
    return ioctlsocket(sock, FIONBIO, &arg);
}

/// ***
/// Determine if socket is readable or writeable
/// ***
const int Scan::Socket::select(fd_set *rfds_ptr, fd_set *wfds_ptr) const
{
    if ((rfds_ptr == nullptr) && (wfds_ptr == nullptr))
    {
        throw NullArgEx({"rfds_ptr", "wfds_ptr"}, true);
        return SOCKET_ERROR;
    }
    timeval to = {3, 500};

    // Determine socket status
    return ::select(NULL, rfds_ptr, wfds_ptr, nullptr,
                                              static_cast<timeval *>(&to));
}

/// ***
/// Prepare socket for connection to destination host
/// ***
addrinfoW *Scan::Socket::startup(SOCKET &sock, const string &port) const
{
    int code;
    WSAData wsadata;

    // Intiialize Winsock2 DLL
    if ((code = WSAStartup(SOCKV, &wsadata)) == INVALID_SOCKET)
    {
        error();
        sock = INVALID_SOCKET;
        return nullptr;
    }

    addrinfoW *ptr = {nullptr}, ai_hints = {
        AI_CANONNAME, AF_INET, SOCK_STREAM, IPPROTO_TCP
    };

    code = GetAddrInfoW(Util::utf16(m_addr).c_str(),
                        Util::utf16(port).c_str(),
                        static_cast<addrinfoW *>(&ai_hints),
                        static_cast<addrinfoW **>(&ptr));

    // Check for DNS lookup errors
    if (code != 0)
    {
        error(EndPoint(m_addr, port).str());
        FreeAddrInfoW(ptr);
        sock = INVALID_SOCKET;
        return nullptr;
    }
    sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    // Socket startup failure
    if (!valid_sock(sock))
    {
        error();
        FreeAddrInfoW(ptr);
        sock = INVALID_SOCKET;
        return nullptr;
    }
    return ptr;
}

/// ***
/// Swap mutable member values with reference object values
/// ***
Scan::Socket &Scan::Socket::swap(const Socket &sock)
{
    m_addr = sock.m_addr;
    m_ports = sock.m_ports;
    m_services = sock.m_services;

    addr = &m_addr;
    ports = &m_ports;

    return *this;
}

/// ***
/// Swap mutable member values with reference object values
/// ***
Scan::Socket &Scan::Socket::swap(const Property<string> &addr,
                                 const Property<vector_s> &ports) {
    m_addr = addr.get();
    m_ports = ports.get();

    this->addr = &m_addr;
    this->ports = &m_ports;

    return *this;
}
