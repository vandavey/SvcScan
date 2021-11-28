/*
*  socket.cpp
*  ----------
*  Source file for an IPv4 TCP network socket
*/
#include <ws2tcpip.h>
#include "includes/except/nullptrex.h"
#include "includes/inet/sockets/socket.h"

#pragma comment(lib, "ws2_32.lib")

/// ***
/// Initialize the object
/// ***
scan::Socket::Socket()
{
    m_timeout = { 3, 500 };
    m_sock = INVALID_SOCKET;

    addr = &m_addr;
    port = &m_port;
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
scan::Socket::Socket(const string &t_addr) : Socket()
{
    m_info.addr = m_addr = t_addr;

    addr = &m_addr;
    port = &m_port;
}

/// ***
/// Destroy the object
/// ***
scan::Socket::~Socket()
{
    net::wsa_cleanup();
    net::free_info();

    // Attempt to close socket descriptor
    if (valid())
    {
        if (closesocket(m_sock) == SOCKET_ERROR)
        {
            std::cerr << "Failed to close underlying socket" << stdu::LF;
        }
        m_sock = INVALID_SOCKET;
    }
}

/// ***
/// Assignment operator overload
/// ***
scan::Socket &scan::Socket::operator=(const Socket &t_sock) noexcept
{
    m_addr = t_sock.m_addr;
    m_port = t_sock.m_port;
    m_sock = t_sock.m_sock;
    m_timeout = t_sock.m_timeout;

    addr = &m_addr;
    port = &m_port;

    return *this;
}

/// ***
/// Assignment operator overload
/// ***
scan::Socket &scan::Socket::operator=(const SOCKET &t_winsock) noexcept
{
    m_sock = t_winsock;

    addr = &m_addr;
    port = &m_port;

    return *this;
}

/// ***
/// Close the underlying socket and reset its handle to default
/// ***
void scan::Socket::close()
{
    close(nullptr);
}

/// ***
/// Set the socket timeout time value
/// ***
void scan::Socket::set_timeout(const uint &t_sec, const uint &t_ms)
{
    m_timeout = { static_cast<long>(t_sec), static_cast<long>(t_ms) };
}

/// ***
/// Establish a TCP connection to the remote host
/// ***
bool scan::Socket::connect(const string &t_addr, const uint &t_port)
{
    const EndPoint ep(t_addr, t_port);
    m_info = ep;

    m_sock = INVALID_SOCKET;

    addrinfoW *ai_ptr{ startup(m_info, t_port) };

    // Invalid socket descriptor
    if (!valid())
    {
        FreeAddrInfoW(ai_ptr);
        return false;
    }

    // Put socket into non-blocking mode
    if (set_blocking(false) != NO_ERROR)
    {
        net::error(ep);
        close(ai_ptr);
        m_info = net::update_svc(m_info, HostState::unknown);
        return false;
    }

    bool connected{ true };
    const int addr_len{ static_cast<int>(ai_ptr->ai_addrlen) };

    // Connect to the remote host
    int rc{ ::connect(m_sock, ai_ptr->ai_addr, addr_len) };

    if (rc == SOCKET_ERROR)
    {
        // Connection attempt failed
        if (int ec{ net::get_error() }; ec != WSAEWOULDBLOCK)
        {
            if (ArgParser::verbose)
            {
                net::error(ep, ec);
            }
            m_info = net::update_svc(m_info, HostState::unknown);
            FreeAddrInfoW(ai_ptr);

            return false;
        }

        fd_set fds{ 1, { m_sock } };
        rc = select(nullptr, &fds, m_timeout);

        // Handle connection failures/timeouts
        if (rc != net::SOCKET_READY)
        {
            if (ArgParser::verbose)
            {
                net::error(ep);
            }
            connected = false;

            // Update service information
            if (rc == SOCKET_ERROR)
            {
                m_info.state = HostState::closed;
            }
            m_info = net::update_svc(m_info, m_info.state);
        }

        // Print connection message
        if (connected && ArgParser::verbose)
        {
            stdu::printf("Connection established: %/tcp", ep.port);
        }
    }
    FreeAddrInfoW(ai_ptr);

    return connected;
}

/// ***
/// Determine whether the underlying socket is valid
/// ***
bool scan::Socket::valid() const noexcept
{
    return net::valid_sock(m_sock);
}

/// ***
/// Retrieve the current timeout used by the underlying socket
/// ***
timeval scan::Socket::get_timeout() const noexcept
{
    return m_timeout;
}

/// ***
/// Retrieve the underlying socket service information
/// ***
scan::SvcInfo scan::Socket::get_svcinfo() const noexcept
{
    return m_info;
}

/// ***
/// Split the payload into strings fragments using the given buffer size
/// ***
scan::Socket::vector_s scan::Socket::split_payload(const string &t_payload,
                                                   const size_t &t_buffer_len) {
    vector_s fragments;

    // Split payload into fragments
    if (!t_payload.empty())
    {
        for (size_t i{ 0 }; i < t_payload.size(); i += t_buffer_len)
        {
            string frag_data{ t_payload.substr(i, i + t_buffer_len) };

            // Trim extra fragment padding
            if ((i + t_buffer_len) >= t_payload.size())
            {
                frag_data = Util::rstrip(frag_data);
            }
            fragments.push_back(frag_data);
        }
    }
    return fragments;
}

/// ***
/// Close the underlying socket and reset its handle to default
/// ***
void scan::Socket::close(addrinfoW *t_aiptr)
{
    if (!valid())
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
        net::error(m_addr);
    }
    m_sock = INVALID_SOCKET;
}

/// ***
/// Poll underlying socket for reading and writing
/// ***
int scan::Socket::select(fd_set *t_read_fdsp,
                         fd_set *t_write_fdsp,
                         const timeval &t_to) const {

    if (!t_read_fdsp && !t_write_fdsp)
    {
        throw NullPtrEx({ "t_read_fdsp", "t_write_fdsp" });
    }

    // Determine if socket is readable/writable
    int rc{ ::select(NULL, t_read_fdsp, t_write_fdsp, nullptr, &t_to) };

    if (rc == NO_ERROR)
    {
        timeval ex_to{ 0, 1 };
        fd_set ex_fds{ 1, m_sock };

        // Handle exception polling results
        switch (rc = ::select(NULL, nullptr, nullptr, &ex_fds, &ex_to))
        {
            case SOCKET_ERROR:  // Error occurred
                return rc;
            case NO_ERROR:      // Timeout occurred
                WSASetLastError(WSAETIMEDOUT);
                return rc;
            default:
                break;
        }

        int ec{ NO_ERROR };
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
    }
    return rc;
}

/// ***
/// Configure blocking options on underlying socket
/// ***
int scan::Socket::set_blocking(const bool &t_do_block)
{
    return net::set_blocking(m_sock, t_do_block);
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

    if (!net::valid_port(t_port))
    {
        throw ArgEx("t_port", "Invalid port number");
    }

    addrinfoW *aiptr{ nullptr };
    addrinfoW ai_hints{ AI_CANONNAME, AF_INET, SOCK_STREAM, IPPROTO_TCP };

    // Avoid WSAHOST_NOT_FOUND false positive
    std::this_thread::sleep_for(Timer::milliseconds(500));

    // Resolve address information
    int rc = GetAddrInfoW(Util::wstr(m_addr).c_str(),
                          std::to_wstring(t_port).c_str(),
                          &ai_hints,
                          &aiptr);

    // Handle DNS lookup errors
    if (rc != NO_ERROR)
    {
        net::error(EndPoint(m_addr, t_port));
        FreeAddrInfoW(aiptr);

        m_sock = INVALID_SOCKET;
        m_info = net::update_svc(t_si, HostState::unknown);

        return nullptr;
    }
    m_sock = socket(aiptr->ai_family, aiptr->ai_socktype, aiptr->ai_protocol);

    // Handle socket startup failure
    if (!valid())
    {
        net::error(EndPoint(m_addr, t_port));
        FreeAddrInfoW(aiptr);

        m_sock = INVALID_SOCKET;
        m_info = net::update_svc(t_si, HostState::unknown);
    }
    return aiptr;
}
