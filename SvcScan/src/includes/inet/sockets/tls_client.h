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
    /**
    * @brief  IPv4 network client with an underlying SSL/TLS socket.
    */
    class TlsClient final : public TcpClient
    {
    private:  /* Type Aliases */
        using base_t = TcpClient;

        using ctx_t        = ssl::context;
        using ssl_stream_t = boost::beast::ssl_stream<stream_t>;
        using verify_cxt_t = ssl::verify_context;

    private:  /* Fields */
        unique_ptr<ctx_t> m_ctxp;                // TLS context smart pointer
        unique_ptr<ssl_stream_t> m_ssl_streamp;  // TLS stream smart pointer

    public:  /* Constructors & Destructor */
        TlsClient() = delete;
        TlsClient(TlsClient &&t_client) noexcept;
        TlsClient(io_context &t_ioc, const Args &t_args);

        virtual ~TlsClient();

    public:  /* Methods */
        void close() override;

        bool valid_handshake() const;

        HostState host_state() const noexcept override;
        HostState host_state(const error_code &t_ecode) const noexcept override;

        OSSL_HANDSHAKE_STATE handshake_state() const;

        size_t recv(char (&t_buffer)[BUFFER_SIZE], error_code &t_ecode) override;

        size_t recv(char (&t_buffer)[BUFFER_SIZE],
                    error_code &t_ecode,
                    const Timeout &t_timeout) override;

        const SSL *connection_ptr() const noexcept;
        const SSL_CIPHER *cipher_ptr() const;

        const X509 *x509_ptr(verify_cxt_t &t_vctx) const;
        const X509_STORE_CTX *x509_ctx_ptr(verify_cxt_t &t_vctx) const;

        error_code handshake();

        error_code send(const string &t_payload) override;
        error_code send(const string &t_payload, const Timeout &t_timeout) override;

        const socket_t &socket() const noexcept override;
        socket_t &socket() noexcept override;

        const stream_t &stream() const noexcept override;
        stream_t &stream() noexcept override;

        string recv(error_code &t_ecode) override;
        string recv(error_code &t_ecode, const Timeout &t_timeout) override;

        Response<> request(const Request<> &t_request) override;

        Response<> request(const string &t_host,
                           const string &t_uri = Request<>::URI_ROOT) override;

        Response<> request(const verb_t &t_method,
                           const string &t_host,
                           const string &t_uri = Request<>::URI_ROOT,
                           const string &t_body = { }) override;

    private:  /* Methods */
        void on_connect(const error_code &t_ecode, Endpoint t_ep) override;
    };
}

#endif // !TLS_CLIENT_H
