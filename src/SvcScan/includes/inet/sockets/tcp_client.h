/*
* @file
*     tcp_client.h
* @brief
*     Header file for an IPv4 TCP socket client.
*/
#pragma once

#ifndef SCAN_TCP_CLIENT_H
#define SCAN_TCP_CLIENT_H

#include <boost/asio/detail/socket_option.hpp>
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

    protected:  /* Fields */
        bool m_connected;                // Client connected
        bool m_verbose;                  // Verbose output

        unique_ptr<stream_t> m_streamp;  // TCP stream smart pointer

        atomic_ptr_t<Args> m_args_ap;    // Command-line arguments atomic pointer
        atomic_ptr_t<TextRc> m_rc_ap;    // Embedded CSV resource atomic pointer

        Timeout m_timeout;               // Connection timeout

        io_context_t& m_io_ctx;          // I/O context reference
        net_error_code_t m_ecode;        // Socket error code

        Endpoint m_remote_ep;            // Remote endpoint
        SvcInfo m_svc_info;              // Service information

    public:  /* Constructors & Destructor */
        TcpClient() = delete;
        TcpClient(const TcpClient&) = delete;
        TcpClient(TcpClient&& t_client) noexcept;

        TcpClient(io_context_t& t_io_ctx,
                  shared_ptr<Args> t_argsp,
                  shared_ptr<TextRc> t_rcp);

        virtual ~TcpClient();

    public:  /* Operators */
        TcpClient& operator=(const TcpClient&) = delete;
        TcpClient& operator=(TcpClient&& t_client) noexcept;

    public:  /* Methods */
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
            HostState state{HostState::closed};

            if (connect_timed_out(m_ecode))
            {
                state = HostState::unknown;
            }
            else if (net::no_error(m_ecode) || operation_timed_out(m_ecode))
            {
                state = HostState::open;
            }
            return state;
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

        virtual void close();
        virtual void connect(const Endpoint& t_ep);
        virtual void connect(port_t t_port);
        void disconnect();
        void shutdown();

        bool is_open() const noexcept;

        virtual size_t recv(buffer_t& t_buffer);
        virtual size_t send(const string& t_payload);

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
        static constexpr bool valid(const net_error_code_t& t_ecode,
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
        constexpr bool connect_timed_out(const net_error_code_t& t_ecode) const noexcept
        {
            return !m_connected && net::timeout_error(t_ecode);
        }

        /**
        * @brief
        *     Determine whether the given socket error
        *     code is a socket operation timeout error.
        */
        constexpr bool operation_timed_out(const net_error_code_t& t_ecode) const noexcept
        {
            return m_connected && net::timeout_error(t_ecode);
        }

        void async_await();
        void async_connect(const results_t& t_results);
        void error(const net_error_code_t& t_ecode);
        virtual void on_connect(const net_error_code_t& t_ecode, Endpoint t_ep);
        void parse_argsp(shared_ptr<Args> t_argsp);
        void recv_timeout(const Timeout& t_timeout);
        void send_timeout(const Timeout& t_timeout);

        template<int SockOpt>
        void set_timeout(const Timeout& t_timeout);

        bool connected_check();
        bool success_check(bool t_allow_eof = true, bool t_allow_partial_msg = true);

        bool success_check(const net_error_code_t& t_ecode,
                           bool t_allow_eof = true,
                           bool t_allow_partial_msg = true);

        virtual const stream_t& stream() const noexcept;
        virtual stream_t& stream() noexcept;

        const Endpoint& remote_ep() const noexcept;

        virtual const socket_t& socket() const noexcept;
        virtual socket_t& socket() noexcept;
    };
}

/**
* @brief
*     Use the given socket option to specify a socket timeout.
*/
template<int SockOpt>
inline void scan::TcpClient::set_timeout(const Timeout& t_timeout)
{
    socket().set_option(socket_option_t<SockOpt>{t_timeout}, m_ecode);
    success_check();
}

#endif // !SCAN_TCP_CLIENT_H
