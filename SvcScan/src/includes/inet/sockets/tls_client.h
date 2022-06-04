/*
*  tls_client.h
*  ------------
*  Header file for a secure IPv4 TCP socket client
*/
#pragma once

#ifndef TLS_CLIENT_H
#define TLS_CLIENT_H

#include <sdkddkver.h>
#include <boost/beast/ssl/ssl_stream.hpp>
#include "tcp_client.h"

namespace
{
    namespace asio = boost::asio;
    namespace ssl  = boost::asio::ssl;
}

namespace scan
{
    /// ***
    /// Secure IPv4 network client using an underlying TCP socket
    /// ***
    class TlsClient final : public TcpClient
    {
    private:  /* Type Aliases */
        using base_t = TcpClient;

        using context_t    = ssl::context;
        using ssl_stream_t = boost::beast::ssl_stream<stream_t>;

    private:  /* Fields */
        unique_ptr<context_t> m_ctxp;            // TLS context smart pointer
        unique_ptr<ssl_stream_t> m_ssl_streamp;  // TLS stream smart pointer

    public:  /* Constructors & Destructor */
        TlsClient() = delete;
        TlsClient(TlsClient &&t_client) noexcept;
        TlsClient(io_context &t_ioc, const Args &t_args);

        virtual ~TlsClient();

    public:  /* Methods */
        error_code handshake();

        error_code send(const string &t_payload,
                        const Timeout &t_timeout = SEND_TIMEOUT) override;

        size_t recv(char (&t_buffer)[BUFFER_SIZE],
                    error_code &t_ecode,
                    const Timeout &t_timeout = RECV_TIMEOUT) override;

        const socket_t &socket() const noexcept override;
        socket_t &socket() noexcept override;

        const stream_t &stream() const noexcept override;
        stream_t &stream() noexcept override;

        string recv(error_code &t_ecode,
                    const Timeout &t_timeout = RECV_TIMEOUT) override;

        Response<> request(const Request<> &t_request) override;

        Response<> request(const verb_t &t_method,
                           const string &t_host,
                           const string &t_uri = "/",
                           const string &t_body = { }) override;

    private:  /* Methods */
        void on_connect(const error_code &t_ecode, Endpoint t_ep) override;
    };
}

#endif // !TLS_CLIENT_H
