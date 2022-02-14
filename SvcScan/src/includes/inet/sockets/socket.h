/*
*  socket.h
*  --------
*  Header file for an IPv4 TCP network socket
*/
#pragma once

#ifndef SOCKET_H
#define SOCKET_H

#ifndef UNICODE
#  define UNICODE 1
#endif // !UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <string>
#include <thread>
#include <winsock2.h>
#include "../../except/logicex.h"
#include "../../utils/args.h"
#include "../../utils/util.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../netutil.h"
#include "../svcinfo.h"
#include "timeout.h"

namespace scan
{
    /// ***
    /// Windows IPv4 TCP network socket wrapper
    /// ***
    class Socket
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using net  = NetUtil;
        using stdu = StdUtil;

        using string   = std::string;
        using sstream  = std::stringstream;
        using vector_s = std::vector<string>;

    public:  /* Constants */
        static constexpr uint CONN_TIMEOUT{ 3500U };  // Default connect() timeout

        static constexpr int BUFFER_LEN{ 1024 };      // Socket buffer size

    private:  /* Constants */
        static constexpr uint POLL_TIMEOUT{ 1U };     // Default select() timeout
        static constexpr uint RECV_TIMEOUT{ 1000U };  // Default recv() timeout

    public:  /* Fields */
        uint port;    // Target port
        string addr;  // Target address

    private:  /* Fields */
        bool m_verbose;          // Verbose console output
        SOCKET m_sock;           // Underlying socket

        Timeout m_conn_timeout;  // Connection timeout

        SvcInfo m_info;          // Service information

    public:  /* Constructors & Destructor */
        Socket();
        Socket(const Socket &t_sock);
        Socket(const Args &t_args);

        virtual ~Socket();

    public:  /* Operators */
        Socket &operator=(const Socket &t_sock) noexcept;
        Socket &operator=(const Args &t_args) noexcept;
        Socket &operator=(const SOCKET &t_winsock) noexcept;

    public:  /* Methods */
        void close();
        void connect_timeout(const Timeout &t_timeout);

        bool connect(const string &t_addr, const uint &t_port);
        bool valid() const noexcept;

        template<int BuffLen = BUFFER_LEN>
        HostState recv(string &t_buffer,
                       const size_t &t_buffer_count = 0,
                       const Timeout &t_timeout = RECV_TIMEOUT);

        template<int BuffLen = BUFFER_LEN>
        int send(const string &t_payload, const size_t &t_buffer_count = 0);

        SvcInfo get_svcinfo() const noexcept;

        template<int BuffLen = BUFFER_LEN>
        Response send(const Request &t_request,
                      const Timeout &t_recv_timeout = RECV_TIMEOUT);

    private:  /* Methods */
        static vector_s split_payload(const string &t_payload,
                                      const size_t &t_buffer_len = BUFFER_LEN);

        void close(addrinfo *t_ai_ptr);

        int select(fd_set *t_rfds_ptr,
                   fd_set *t_wfds_ptr,
                   const Timeout &t_timeout = POLL_TIMEOUT) const;

        int set_blocking(const bool &t_do_block);

        addrinfo *startup(SvcInfo &t_si, const uint &t_port);
    };
}

/// ***
/// Read inbound data from the underlying socket stream
/// ***
template<int BuffLen>
inline scan::HostState scan::Socket::recv(string &t_buffer,
                                          const size_t &t_buffer_count,
                                          const Timeout &t_timeout) {

    static_assert(BuffLen > 0, "The socket buffer size must be greater than 0");

    if (!valid())
    {
        throw LogicEx{ "Socket::recv", "Invalid underlying socket" };
    }

    fd_set fds{ 1, { m_sock } };
    HostState hs{ HostState::unknown };

    bool readable_stream{ false };

    // Poll connected socket for readability
    switch (select(&fds, nullptr, t_timeout))
    {
        case SOCKET_ERROR:       // Socket failure
        {
            if (m_verbose)
            {
                net::error(addr);
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
            readable_stream = true;
            break;
        }
        default:
        {
            break;
        }
    }

    // Socket stream is readable
    if (readable_stream)
    {
        sstream ss;

        // Wait for inbound data
        std::this_thread::sleep_for(Timer::milliseconds(200));

        size_t buffer_count{ 0 };
        int bytes_read{ SOCKET_ERROR };

        do  // Read inbound socket stream data
        {
            if ((t_buffer_count != 0) && (buffer_count >= t_buffer_count))
            {
                break;
            }
            char buffer[BuffLen]{ NULL };

            bytes_read = ::recv(m_sock, &buffer[0], BuffLen, NULL);

            // Break if no more data is available
            if (bytes_read == SOCKET_ERROR)
            {
                if (buffer_count == 0)
                {
                    net::error(addr);
                }
                break;
            }

            buffer_count++;
            ss << &buffer[0];
        }
        while (bytes_read > 0);

        t_buffer = ss.str();
        hs = HostState::open;
    }
    return hs;
}

/// ***
/// Send all contents of the entire given string payload
/// ***
template<int BuffLen>
inline int scan::Socket::send(const string &t_payload, const size_t &t_buffer_count)
{
    static_assert(BuffLen > 0, "The socket buffer size must be greater than 0");

    if (!valid())
    {
        throw LogicEx{ "Socket::send", "Invalid underlying socket" };
    }

    fd_set fds{ 1, { m_sock } };

    int total_bytes_sent{ 0 };
    bool is_writable_stream{ false };

    // Poll connected socket for writability
    switch (select(nullptr, &fds, POLL_TIMEOUT))
    {
        case SOCKET_ERROR:       // Socket failure
        {
            if (m_verbose)
            {
                net::error(addr);
            }
            total_bytes_sent = SOCKET_ERROR;
            break;
        }
        case net::SOCKET_READY:  // Writable stream
        {
            is_writable_stream = true;
            break;
        }
        default:
        {
            total_bytes_sent = SOCKET_ERROR;
            break;
        }
    }

    // Socket stream is writable
    if (is_writable_stream)
    {
        size_t buffer_count{ 0 };
        const vector_s fragments{ split_payload(t_payload, BuffLen) };

        // Send fragmented payload data
        for (const string &fragment : fragments)
        {
            if ((t_buffer_count != 0) && (buffer_count >= t_buffer_count))
            {
                break;
            }

            const string frag{ Util::rstrip(fragment) };
            const int frag_size{ static_cast<int>(frag.size()) };

            const int bytes_sent{ ::send(m_sock, frag.c_str(), frag_size, NULL) };

            // Failed to send payload fragment
            if (bytes_sent == SOCKET_ERROR)
            {
                if (m_verbose)
                {
                    net::error(addr);
                }
                total_bytes_sent = SOCKET_ERROR;
                break;
            }

            total_bytes_sent += bytes_sent;
            buffer_count++;
        }
    }
    return total_bytes_sent;
}

/// ***
/// Send a raw HTTP request over the underlying socket stream
/// ***
template<int BuffLen>
inline scan::Response scan::Socket::send(const Request &t_request,
                                         const Timeout &t_recv_timeout) {

    static_assert(BuffLen > 0, "The socket buffer size must be greater than 0");

    Response response;
    const string raw_request{ t_request };

    const int total_bytes_sent{ send<BuffLen>(raw_request) };

    // Read inbound HTTP response data
    if (total_bytes_sent > 0)
    {
        string recv_buffer;
        const HostState hs{ recv(recv_buffer, t_recv_timeout) };

        // Process HTTP response data
        if (hs == HostState::open)
        {
            response.parse(recv_buffer);
        }
    }
    return response;
}

#endif // !SOCKET_H
