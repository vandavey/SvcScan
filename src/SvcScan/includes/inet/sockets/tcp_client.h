/*
* @file
*     tcp_client.h
* @brief
*     Header file for an IPv4 TCP socket client.
*/
#pragma once

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <sdkddkver.h>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include "../../contracts/i_args_parser.h"
#include "../../errors/null_arg_ex.h"
#include "../../utils/type_defs.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../net_expr.h"
#include "endpoint.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 network client with an underlying TCP socket.
    */
    class TcpClient : public IArgsParser
    {
    public:  /* Type Aliases */
        using buffer_t = array<char, BUFFER_SIZE>;

    protected:  /* Type Aliases */
        using net         = NetUtil;
        using response_t  = http::response<string_body>;

    private:  /* Type Aliases */
        using this_t = TcpClient;

    protected:  /* Fields */
        bool m_connected;                // Client connected
        bool m_verbose;                  // Verbose output

        unique_ptr<stream_t> m_streamp;  // TCP stream smart pointer

        atomic_ptr<Args> m_args_ap;      // Command-line arguments smart pointer
        atomic_ptr<TextRc> m_trc_ap;     // Embedded CSV resource smart pointer

        Timeout m_conn_timeout;          // Connection timeout
        Timeout m_recv_timeout;          // Receive timeout
        Timeout m_send_timeout;          // Send timeout

        io_context &m_ioc;               // I/O context reference
        error_code m_ecode;              // Socket error code

        Endpoint m_remote_ep;            // Remote endpoint
        SvcInfo m_svc_info;              // Service information

    public:  /* Constructors & Destructor */
        TcpClient() = delete;
        TcpClient(const TcpClient &) = default;
        TcpClient(TcpClient &&t_client) noexcept;
        TcpClient(io_context &t_ioc, shared_ptr<Args> t_argsp, shared_ptr<TextRc> t_trcp);

        virtual ~TcpClient();

    public:  /* Operators */
        TcpClient &operator=(const TcpClient &) = default;
        TcpClient &operator=(TcpClient &&t_client) noexcept;

    public:  /* Methods */
        void async_connect(const results_t &t_results,
                           const Timeout &t_timeout = CONN_TIMEOUT);

        void await_task();
        virtual void close();
        virtual void connect(const Endpoint &t_ep);
        virtual void connect(const port_t &t_port);
        void connect_timeout(const Timeout &t_timeout);
        void disconnect();
        void parse_argsp(shared_ptr<Args> t_argsp) override;
        void recv_timeout(const Timeout &t_timeout);
        void send_timeout(const Timeout &t_timeout);
        void shutdown();

        bool is_connected() const noexcept;
        bool is_open() const noexcept;

        virtual HostState host_state() const noexcept;
        virtual HostState host_state(const error_code &t_ecode) const noexcept;

        virtual size_t recv(buffer_t &t_buffer);
        virtual size_t recv(buffer_t &t_buffer, error_code &t_ecode);

        virtual size_t recv(buffer_t &t_buffer,
                            error_code &t_ecode,
                            const Timeout &t_timeout);

        virtual const stream_t &stream() const noexcept;
        virtual stream_t &stream() noexcept;

        error_code last_error() const noexcept;
        virtual error_code send(const string &t_payload);
        virtual error_code send(const string &t_payload, const Timeout &t_timeout);

        virtual const socket_t &socket() const noexcept;
        virtual socket_t &socket() noexcept;

        virtual string recv();
        virtual string recv(error_code &t_ecode);
        virtual string recv(error_code &t_ecode, const Timeout &t_timeout);

        const Endpoint &remote_ep() const noexcept;

        const SvcInfo &svcinfo() const noexcept;
        SvcInfo &svcinfo() noexcept;

        virtual Response<> request(const Request<> &t_request);
        virtual Response<> request(const string &t_host, const string &t_uri = URI_ROOT);

        virtual Response<> request(const verb_t &t_method,
                                   const string &t_host,
                                   const string &t_uri = URI_ROOT,
                                   const string &t_body = { });

    protected:  /* Methods */
        void error(const error_code &t_ecode);
        virtual void on_connect(const error_code &t_ecode, Endpoint t_ep);

        template<int SockOpt>
        void set_timeout(const Timeout &t_timeout);

        bool connected_check();

        bool success_check(const bool &t_allow_eof = true,
                           const bool &t_allow_partial = true);

        bool success_check(const error_code &t_ecode,
                           const bool &t_allow_eof = true,
                           const bool &t_allow_partial = true);

        virtual bool valid(const error_code &t_ecode,
                           const bool &t_allow_eof = true,
                           const bool &t_allow_partial = true) noexcept;
    };
}

/**
* @brief
*     Use the given socket option to specify a socket timeout.
*/
template<int SockOpt>
inline void scan::TcpClient::set_timeout(const Timeout &t_timeout)
{
    socket().set_option(sock_opt<SockOpt>(t_timeout), m_ecode);
    success_check();
}

#endif // !TCP_CLIENT_H
