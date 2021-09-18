/*
*  socket.cpp
*  ----------
*  Source file for an IPv4 TCP network socket
*/
#include <thread>
#include <ws2tcpip.h>
#include "includes/containers/svctable.h"
#include "includes/except/nullptrex.h"
#include "includes/filesys/filestream.h"
#include "includes/inet/endpoint.h"
#include "includes/inet/socket.h"

#pragma comment(lib, "ws2_32.lib")

scan::AutoProp<std::string> scan::Socket::out_path;

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
    net::wsa_cleanup();
    net::free_info();
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
/// Connect to the remote host
/// ***
void scan::Socket::connect()
{
    // Initialize use of WinSock DLL
    if (net::wsa_startup(m_addr) != NO_ERROR)
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

    // Print scan start message
    std::cout << Util::fstr("Beginning SvcScan (%)", Parser::REPO) << stdu::LF
              << "Time: "   << Timer::timestamp(m_timer.start())   << stdu::LF
              << "Target: " << m_addr                              << stdu::LF
              << "Ports: '" << ports_str << "'"                    << stdu::LF;

    if (Parser::verbose)
    {
        std::cout << stdu::LF;
    }

    // Connect to each port in underlying ports list
    for (const uint &port : m_ports)
    {
        m_sock = INVALID_SOCKET;
        const EndPoint ep(m_addr, port);

        SvcInfo si{ ep };
        addrinfoW *ai_ptr{ startup(si, port) };

        // Invalid socket descriptor
        if (!net::valid_sock(m_sock))
        {
            FreeAddrInfoW(ai_ptr);
            continue;
        }

        // Put socket into non-blocking mode
        if (set_blocking(false) != NO_ERROR)
        {
            net::error(ep);
            close(ai_ptr);
            m_services.add(net::update_svc(si, HostState::unknown));
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

        net::update_svc(si, hs);
        m_services.add(si);

        // Close socket and free addrinfoW
        close(ai_ptr);
    }

    m_timer.stop();

    const SvcTable table(m_addr, m_services);
    const string summary{ net::scan_summary(m_addr, m_timer, out_path) };

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
void scan::Socket::close(addrinfoW *t_aiptr)
{
    if (!net::valid_sock(m_sock))
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
/// Connect to the remote host and read banner data
/// ***
scan::HostState scan::Socket::connect(addrinfoW *t_aiptr,
                                      char (&t_buffer)[BUFFER_SIZE],
                                      const EndPoint &t_ep) {
    if (!net::valid_sock(m_sock))
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
        if (int ec{ net::get_error() }; ec != WSAEWOULDBLOCK)
        {
            if (Parser::verbose)
            {
                net::error(t_ep, ec);
            }
            return HostState::closed;
        }

        fd_set fds{ 1, { m_sock } };
        rc = select(nullptr, &fds, m_timeout);

        // Handle connection failures/timeouts
        if (rc != net::SOCKET_READY)
        {
            if (Parser::verbose)
            {
                net::error(t_ep);
            }
            return (rc == SOCKET_ERROR) ? HostState::closed : HostState::unknown;
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

    if (!net::valid_sock(m_sock))
    {
        throw LogicEx("Socket::recv", "Invalid underlying socket");
    }

    fd_set fds{ 1, { m_sock } };
    HostState hs{ HostState::unknown };

    // Poll connected socket for readability
    switch (select(&fds, nullptr, timeval{ 1, 0 }))
    {
        case SOCKET_ERROR:       // Socket failure
        {
            if (Parser::verbose)
            {
                net::error(m_addr);
            }
            hs = HostState::closed;
            break;
        }
        case NO_ERROR:           // Unreadable stream
        {
            hs = HostState::open;
            break;
        }
        case net::SOCKET_READY:  // Readable stream
        {
            // Wait for inbound data
            Sleep(200);

            // Read inbound socket data
            if (::recv(m_sock, t_buffer, BUFFER_SIZE, 0) == SOCKET_ERROR)
            {
                net::error(m_addr);
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
    int rc;

    addrinfoW *aiptr{ nullptr };
    addrinfoW ai_hints{ AI_CANONNAME, AF_INET, SOCK_STREAM, IPPROTO_TCP };

    // Avoid WSAHOST_NOT_FOUND false positive
    std::this_thread::sleep_for(Timer::milliseconds(500));

    // Resolve address information
    rc = GetAddrInfoW(Util::wstr(m_addr).c_str(),
                      std::to_wstring(t_port).c_str(),
                      &ai_hints,
                      &aiptr);

    // Handle DNS lookup errors
    if (rc != NO_ERROR)
    {
        net::error(EndPoint(m_addr, t_port));
        FreeAddrInfoW(aiptr);

        m_sock = INVALID_SOCKET;
        m_services.add(net::update_svc(t_si, HostState::unknown));

        return nullptr;
    }
    m_sock = socket(aiptr->ai_family, aiptr->ai_socktype, aiptr->ai_protocol);

    // Handle socket startup failure
    if (!net::valid_sock(m_sock))
    {
        net::error(EndPoint(m_addr, t_port));
        FreeAddrInfoW(aiptr);
        m_sock = INVALID_SOCKET;

        m_services.add(net::update_svc(t_si, HostState::unknown));
        aiptr = nullptr;
    }
    return aiptr;
}
