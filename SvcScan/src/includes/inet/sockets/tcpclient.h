/*
*  tcpclient.h
*  -----------
*  Header file for an IPv4 TCP network client
*/
#pragma once

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include "../../contracts/iargsparser.h"
#include "../../except/nullargex.h"
#include "../http/request.h"
#include "../http/response.h"

namespace
{
    namespace asio = boost::asio;
    namespace http = boost::beast::http;
}

namespace scan
{
    /// ***
    /// IPv4 network client using an underlying TCP socket
    /// ***
    class TcpClient : public IArgsParser
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using error_code = boost::system::error_code;
        using io_context = asio::io_context;
        using net        = NetUtil;
        using response_t = http::response<http::string_body>;
        using results_t  = asio::ip::tcp::resolver::results_type;
        using socket_t   = asio::ip::tcp::socket;
        using string     = std::string;
        using tcp        = asio::ip::tcp;
        using tcp_ep     = tcp::endpoint;
        using tcp_stream = boost::beast::tcp_stream;
        using verb       = http::verb;

        template<int OptName>
        using sockopt = asio::detail::socket_option::integer<SOL_SOCKET, OptName>;

    public:  /* Constants */
        static constexpr uint CONN_TIMEOUT{ 3500U };  // Default connect() timeout
        static constexpr size_t BUFFER_SIZE{ 1024 };  // Default buffer size

    private:  /* Constants */
        static constexpr uint RECV_TIMEOUT{ 1000U };  // Default recv() timeout
        static constexpr uint SEND_TIMEOUT{ 500U };   // Default send() timeout

    private:  /* Fields */
        bool m_connected;        // Client connected
        bool m_verbose;          // Verbose output

        error_code m_ecode;      // Socket error code

        Endpoint m_remote_ep;    // Remote endpoint
        TextRc m_csv_rc;         // Embedded CSV resource

        io_context &m_ioc;       // I/O context reference
        tcp_stream m_stream;     // TCP network socket

        Timeout m_conn_timeout;  // Connection timeout
        SvcInfo m_svc_info;      // Service information

    public:  /* Constructors & Destructor */
        TcpClient() = delete;
        TcpClient(TcpClient &&t_client) noexcept;
        TcpClient(io_context &t_ioc, const Args &t_args);

        virtual ~TcpClient();

    public:  /* Methods */
        void await_operation();
        void close();
        void connect(const string &t_addr, const uint &t_port);
        void connect(const uint &t_port);
        void connect_timeout(const Timeout &t_timeout);
        void disconnect();
        void parse_args(const Args &t_args) noexcept override;
        void recv_timeout(const Timeout &t_timeout);
        void send_timeout(const Timeout &t_timeout);
        void shutdown();

        bool is_connected() const noexcept;
        bool is_open() const noexcept;

        error_code last_error() const noexcept;

        error_code send(const string &t_payload,
                        const Timeout &t_timeout = SEND_TIMEOUT);

        template<size_t N>
        size_t recv(char (&t_buffer)[N],
                    error_code &t_ecode,
                    const Timeout &t_timeout = RECV_TIMEOUT);

        string recv(error_code &t_ecode, const Timeout &t_timeout = RECV_TIMEOUT);

        const SvcInfo &svcinfo() const noexcept;
        SvcInfo &svcinfo() noexcept;

        const TextRc &textrc() const noexcept;

        template<class T = http::string_body>
        Response<T> request(const Request<T> &t_request);

        template<class T = http::string_body>
        Response<T> request(const verb &t_method,
                            const string &t_host,
                            const string &t_uri = "/",
                            const string &t_body = string());

    private:  /* Methods */
        static bool valid(const error_code &t_ecode,
                          const bool &t_eof_valid = true) noexcept;

        void error(const error_code &t_ecode);
        void on_connect(const error_code &t_ecode, Endpoint t_ep);

        template<int SockOpt>
        void set_timeout(const Timeout &t_timeout);

        bool connected_check();
        bool success_check();
        bool success_check(const error_code &t_ecode);

        error_code connect(const results_t &t_results,
                           const Timeout &t_timeout = CONN_TIMEOUT);

        const socket_t &socket() const noexcept;
        socket_t &socket() noexcept;
    };
}
/// ***
/// Read inbound data from the underlying socket stream
/// ***
template<size_t N>
inline size_t scan::TcpClient::recv(char (&t_buffer)[N],
                                    error_code &t_ecode,
                                    const Timeout &t_timeout) {
    if (t_buffer == NULL)
    {
        throw NullArgEx{ "t_buffer" };
    }

    string data;
    size_t bytes_read{ 0U };

    // Read stream data
    if (connected_check())
    {
        recv_timeout(t_timeout);

        bytes_read = m_stream.read_some(asio::buffer(t_buffer, N), t_ecode);
        m_ecode = t_ecode;
    }
    return bytes_read;
}

/// ***
/// Send the given HTTP request and return the server's response
/// ***
template<class T>
inline scan::Response<T> scan::TcpClient::request(const Request<T> &t_request)
{
    if (!t_request.valid())
    {
        throw ArgEx{ "t_request", "Invalid request" };
    }
    Response<T> response;

    if (connected_check())
    {
        // Send the HTTP request
        http::write(m_stream, t_request.request(), m_ecode);

        if (success_check(m_ecode))
        {
            response_t resp;

            // Read the HTTP response
            http::read(m_stream, response.buffer, resp, m_ecode);

            if (success_check(m_ecode))
            {
                response.parse(resp);
            }
        }
    }
    return response;
}

/// ***
/// Send a HTTP request and return the server's response
/// ***
template<class T>
inline scan::Response<T> scan::TcpClient::request(const verb &t_method,
                                                  const string &t_host,
                                                  const string &t_uri,
                                                  const string &t_body) {
    Response<T> response;

    if (connected_check())
    {
        response = request(Request{ t_method, t_host, t_uri, t_body });
    }
    return response;
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
