/*
* @file
*     tls_client.h
* @brief
*     Header file for a secure IPv4 TCP socket client.
*/
#pragma once

#ifndef SCAN_TLS_CLIENT_H
#define SCAN_TLS_CLIENT_H

#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/verify_context.hpp>
#include <openssl/ssl.h>
#include "../../resources/text_rc.h"
#include "../../utils/args.h"
#include "../../utils/type_defs.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../net_defs.h"
#include "../net_expr.h"
#include "endpoint.h"
#include "host_state.h"
#include "tcp_client.h"
#include "timeout.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 network client with an underlying SSL/TLS socket.
    */
    class TlsClient final : public TcpClient
    {
    private:  /* Type Aliases */
        using base_t = TcpClient;

        using cipher_t     = SSL_CIPHER;
        using ctx_t        = ssl::context;
        using verify_ctx_t = ssl::verify_context;

    private:  /* Fields */
        unique_ptr<ctx_t> m_ctxp;                // TLS context smart pointer
        unique_ptr<ssl_stream_t> m_ssl_streamp;  // TLS stream smart pointer

    public:  /* Constructors & Destructor */
        TlsClient() = delete;
        TlsClient(const TlsClient &) = default;
        TlsClient(TlsClient &&t_client) noexcept;
        TlsClient(io_context &t_ioc, shared_ptr<Args> t_argsp, shared_ptr<TextRc> t_trcp);

        virtual ~TlsClient();

    public:  /* Operators */
        TlsClient &operator=(const TlsClient &) = default;
        TlsClient &operator=(TlsClient &&t_client) noexcept;

    public:  /* Methods */
        void async_handshake(const Timeout &t_timeout = RECV_TIMEOUT);
        void close() override;
        void connect(const Endpoint &t_ep) override;
        void connect(const port_t &t_port) override;

        bool valid_handshake() const;

        HostState host_state() const noexcept override;
        HostState host_state(const error_code &t_ecode) const noexcept override;

        OSSL_HANDSHAKE_STATE handshake_state() const;

        size_t recv(buffer_t &t_buffer) override;
        size_t recv(buffer_t &t_buffer, error_code &t_ecode) override;

        size_t recv(buffer_t &t_buffer,
                    error_code &t_ecode,
                    const Timeout &t_timeout) override;

        const cipher_t *cipher_ptr() const;

        const stream_t &stream() const noexcept override;
        stream_t &stream() noexcept override;

        error_code handshake();
        error_code send(const string &t_payload) override;
        error_code send(const string &t_payload, const Timeout &t_timeout) override;

        const socket_t &socket() const noexcept override;
        socket_t &socket() noexcept override;

        string cipher_suite() const;
        string recv() override;
        string recv(error_code &t_ecode) override;
        string recv(error_code &t_ecode, const Timeout &t_timeout) override;

        Response<> request(const Request<> &t_request) override;
        Response<> request(const string &t_host, const string &t_uri = URI_ROOT) override;

        Response<> request(const verb_t &t_method,
                           const string &t_host,
                           const string &t_uri = URI_ROOT,
                           const string &t_body = {}) override;

    private:  /* Methods */
        void on_connect(const error_code &t_ecode, Endpoint t_ep) override;
        void on_handshake(const error_code &t_ecode);

        bool on_verify(bool t_preverified, verify_ctx_t &t_verify_ctx);

        bool valid(const error_code &t_ecode,
                   const bool &t_allow_eof = true,
                   const bool &t_allow_partial = true) noexcept override;
    };
}

#endif // !SCAN_TLS_CLIENT_H
