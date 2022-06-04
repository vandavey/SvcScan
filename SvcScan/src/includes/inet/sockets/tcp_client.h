/*
*  tcp_client.h
*  ------------
*  Header file for an IPv4 TCP socket client
*/
#pragma once

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include "../../contracts/i_args_parser.h"
#include "../../except/null_arg_ex.h"
#include "../http/request.h"
#include "../http/response.h"

namespace
{
    namespace asio = boost::asio;
    namespace http = boost::beast::http;
    namespace ssl  = asio::ssl;
}

namespace scan
{
    /// ***
    /// IPv4 network client using an underlying TCP socket
    /// ***
    class TcpClient : public IArgsParser
    {
    protected:  /* Type Aliases */
        using uint = unsigned int;

        using error_code = boost::system::error_code;
        using io_context = asio::io_context;
        using net        = NetUtil;
        using response_t = http::response<http::string_body>;
        using results_t  = asio::ip::tcp::resolver::results_type;
        using socket_t   = asio::ip::tcp::socket;
        using stream_t   = boost::beast::tcp_stream;
        using string     = std::string;
        using verb_t     = http::verb;

        template<int OptName>
        using sockopt = asio::detail::socket_option::integer<SOL_SOCKET, OptName>;

        template<class T>
        using unique_ptr = std::unique_ptr<T>;

    public:  /* Constants */
        static constexpr uint CONN_TIMEOUT{ 3500U };  // Default connect() timeout
        static constexpr size_t BUFFER_SIZE{ 1024 };  // Default buffer size

    protected:  /* Constants */
        static constexpr uint RECV_TIMEOUT{ 1000U };  // Default recv() timeout
        static constexpr uint SEND_TIMEOUT{ 500U };   // Default send() timeout

    protected:  /* Fields */
        bool m_connected;                // Client connected
        bool m_verbose;                  // Verbose output

        error_code m_ecode;              // Socket error code

        Endpoint m_remote_ep;            // Remote endpoint
        Timeout m_conn_timeout;          // Connection timeout

        SvcInfo m_svc_info;              // Service information
        TextRc m_csv_rc;                 // Embedded CSV resource

        io_context &m_ioc;               // I/O context reference
        unique_ptr<stream_t> m_streamp;  // TCP stream smart pointer

    public:  /* Constructors & Destructor */
        TcpClient() = delete;
        TcpClient(TcpClient &&t_client) noexcept;
        TcpClient(io_context &t_ioc, const Args &t_args);

        virtual ~TcpClient();

    public:  /* Methods */
        void await_operation(const bool &t_restart = true);
        void close();
        virtual void connect(const Endpoint &t_ep);
        virtual void connect(const uint &t_port);
        void connect_timeout(const Timeout &t_timeout);
        void disconnect();
        void parse_args(const Args &t_args) noexcept override;
        void recv_timeout(const Timeout &t_timeout);
        void send_timeout(const Timeout &t_timeout);
        void shutdown();

        bool is_connected() const noexcept;
        bool is_open() const noexcept;

        error_code last_error() const noexcept;

        virtual error_code send(const string &t_payload,
                                const Timeout &t_timeout = SEND_TIMEOUT);

        virtual size_t recv(char (&t_buffer)[BUFFER_SIZE],
                            error_code &t_ecode,
                            const Timeout &t_timeout = RECV_TIMEOUT);

        virtual const socket_t &socket() const noexcept;
        virtual socket_t &socket() noexcept;

        virtual const stream_t &stream() const noexcept;
        virtual stream_t &stream() noexcept;

        virtual string recv(error_code &t_ecode,
                            const Timeout &t_timeout = RECV_TIMEOUT);

        const SvcInfo &svcinfo() const noexcept;
        SvcInfo &svcinfo() noexcept;

        const TextRc &textrc() const noexcept;

        virtual Response<> request(const Request<> &t_request);

        virtual Response<> request(const verb_t &t_method,
                                   const string &t_host,
                                   const string &t_uri = "/",
                                   const string &t_body = { });

    protected:  /* Methods */
        static bool valid(const error_code &t_ecode,
                          const bool &t_eof_valid = true) noexcept;

        void error(const error_code &t_ecode);
        virtual void on_connect(const error_code &t_ecode, Endpoint t_ep);

        template<int SockOpt>
        void set_timeout(const Timeout &t_timeout);

        bool connected_check();
        bool success_check();
        bool success_check(const error_code &t_ecode);

        virtual error_code connect(const results_t &t_results,
                                   const Timeout &t_timeout = CONN_TIMEOUT);
    };
}

/// ***
/// Use the given socket option to specify a TCP operation timeout
/// ***
template<int SockOpt>
inline void scan::TcpClient::set_timeout(const Timeout &t_timeout)
{
    socket().set_option(sockopt<SockOpt>(t_timeout), m_ecode);
    success_check(m_ecode);
}

#endif // !TCP_CLIENT_H
