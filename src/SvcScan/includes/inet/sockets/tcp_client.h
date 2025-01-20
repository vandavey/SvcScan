/*
* @file
*     tcp_client.h
* @brief
*     Header file for an IPv4 TCP socket client.
*/
#pragma once

#ifndef SCAN_TCP_CLIENT_H
#define SCAN_TCP_CLIENT_H

#include <boost/beast/http/message.hpp>
#include "../../console/args.h"
#include "../../resources/text_rc.h"
#include "../../threading/thread_aliases.h"
#include "../../utils/aliases.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../net.h"
#include "../net_aliases.h"
#include "../net_const_defs.h"
#include "../services/svc_info.h"
#include "endpoint.h"
#include "host_state.h"
#include "timeout.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 TCP socket client.
    */
    class TcpClient
    {
    public:  /* Type Aliases */
        using buffer_t = array<char, BUFFER_SIZE>;

    protected:  /* Type Aliases */
        using response_t = http::response<string_body>;

    protected:  /* Fields */
        bool m_connected;                // Client connected
        bool m_verbose;                  // Verbose output

        unique_ptr<stream_t> m_streamp;  // TCP stream smart pointer

        // Command-line arguments atomic shared pointer
        atomic_shared_ptr<Args> m_args_ap;

        // Embedded CSV resource atomic shared pointer
        atomic_shared_ptr<TextRc> m_trc_ap;

        Timeout m_conn_timeout;          // Connection timeout
        Timeout m_recv_timeout;          // Receive timeout
        Timeout m_send_timeout;          // Send timeout

        io_context& m_ioc;               // I/O context reference
        error_code m_ecode;              // Socket error code

        Endpoint m_remote_ep;            // Remote endpoint
        SvcInfo m_svc_info;              // Service information

    public:  /* Constructors & Destructor */
        TcpClient() = delete;
        TcpClient(const TcpClient&) = default;
        TcpClient(TcpClient&& t_client) noexcept;
        TcpClient(io_context& t_ioc, shared_ptr<Args> t_argsp, shared_ptr<TextRc> t_trcp);

        virtual ~TcpClient();

    public:  /* Operators */
        TcpClient& operator=(const TcpClient&) = default;
        TcpClient& operator=(TcpClient&& t_client) noexcept;

    public:  /* Methods */
        /**
        * @brief
        *     Set the socket connection timeout duration.
        */
        constexpr void connect_timeout(const Timeout& t_timeout) noexcept
        {
            m_conn_timeout = t_timeout;
        }

        /**
        * @brief
        *     Determine whether the underlying TCP socket is connected.
        */
        constexpr bool is_connected() const noexcept
        {
            return m_connected;
        }

        /**
        * @brief
        *     Get the remote host state based on the last socket error code.
        */
        virtual constexpr HostState host_state() const noexcept
        {
            return host_state(m_ecode);
        }

        /**
        * @brief
        *     Get the remote host state based on the given socket error code.
        */
        virtual constexpr HostState host_state(const error_code& t_ecode) const noexcept
        {
            HostState state{HostState::closed};

            if (connect_timed_out(t_ecode))
            {
                state = HostState::unknown;
            }
            else if (net::no_error(t_ecode) || operation_timed_out(t_ecode))
            {
                state = HostState::open;
            }
            return state;
        }

        /**
        * @brief
        *     Get the most recent socket error code.
        */
        constexpr error_code last_error() const noexcept
        {
            return m_ecode;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying network app service information.
        */
        constexpr const SvcInfo& svcinfo() const noexcept
        {
            return m_svc_info;
        }

        /**
        * @brief
        *     Get a reference to the underlying network app service information.
        */
        constexpr SvcInfo& svcinfo() noexcept
        {
            return m_svc_info;
        }

        void async_await();

        void async_connect(const results_t& t_results,
                           const Timeout& t_timeout = CONN_TIMEOUT);

        virtual void close();
        virtual void connect(const Endpoint& t_ep);
        virtual void connect(port_t t_port);
        void disconnect();
        void parse_argsp(shared_ptr<Args> t_argsp);
        void recv_timeout(const Timeout& t_timeout);
        void send_timeout(const Timeout& t_timeout);
        void shutdown();

        bool is_open() const noexcept;

        virtual size_t recv(buffer_t& t_buffer);
        virtual size_t recv(buffer_t& t_buffer, error_code& t_ecode);

        virtual size_t recv(buffer_t& t_buffer,
                            error_code& t_ecode,
                            const Timeout& t_timeout);

        virtual const stream_t& stream() const noexcept;
        virtual stream_t& stream() noexcept;

        virtual error_code send(const string& t_payload);
        virtual error_code send(const string& t_payload, const Timeout& t_timeout);

        virtual const socket_t& socket() const noexcept;
        virtual socket_t& socket() noexcept;

        virtual string recv();
        virtual string recv(error_code& t_ecode);
        virtual string recv(error_code& t_ecode, const Timeout& t_timeout);

        const Endpoint& remote_ep() const noexcept;

        virtual Response<> request(const Request<>& t_request);
        virtual Response<> request(const string& t_host, const string& t_uri = URI_ROOT);

        virtual Response<> request(verb_t t_method,
                                   const string& t_host,
                                   const string& t_uri = URI_ROOT,
                                   const string& t_body = {});

    protected:  /* Methods */
        /**
        * @brief
        *     Determine whether the client is in a valid
        *     state based on the given socket error code.
        */
        static constexpr bool valid(const error_code& t_ecode,
                                    bool t_allow_eof = true,
                                    bool t_allow_partial_msg = true)
            noexcept
        {
            return net::no_error(t_ecode, t_allow_eof, t_allow_partial_msg);
        }

        /**
        * @brief
        *     Determine whether the given socket error code is a connection timeout error.
        */
        constexpr bool connect_timed_out(const error_code& t_ecode) const noexcept
        {
            return !m_connected && net::timeout_error(t_ecode);
        }

        /**
        * @brief
        *     Determine whether the given socket error
        *     code is a socket operation timeout error.
        */
        constexpr bool operation_timed_out(const error_code& t_ecode) const noexcept
        {
            return m_connected && net::timeout_error(t_ecode);
        }

        void error(const error_code& t_ecode);
        virtual void on_connect(const error_code& t_ecode, Endpoint t_ep);

        template<int SockOpt>
        void set_timeout(const Timeout& t_timeout);

        bool connected_check();
        bool success_check(bool t_allow_eof = true, bool t_allow_partial_msg = true);

        bool success_check(const error_code& t_ecode,
                           bool t_allow_eof = true,
                           bool t_allow_partial_msg = true);
    };
}

/**
* @brief
*     Use the given socket option to specify a socket timeout.
*/
template<int SockOpt>
inline void scan::TcpClient::set_timeout(const Timeout& t_timeout)
{
    socket().set_option(socket_option<SockOpt>(t_timeout), m_ecode);
    success_check();
}

#endif // !SCAN_TCP_CLIENT_H
