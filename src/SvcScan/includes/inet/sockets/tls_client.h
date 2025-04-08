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
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/verify_context.hpp>
#include <openssl/ssl.h>
#include "../../console/args.h"
#include "../../resources/text_rc.h"
#include "../../utils/aliases.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../net.h"
#include "../net_aliases.h"
#include "../net_const_defs.h"
#include "endpoint.h"
#include "host_state.h"
#include "tcp_client.h"
#include "timeout.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 TCP socket client that supports SSL/TLS protocols.
    */
    class TlsClient final : public TcpClient
    {
    private:  /* Type Aliases */
        using ssl_context_t    = ssl::context;
        using verify_context_t = ssl::verify_context;

    private:  /* Fields */
        unique_ptr<ssl_context_t> m_ssl_ctxp;    // TLS context smart pointer
        unique_ptr<ssl_stream_t> m_ssl_streamp;  // TLS stream smart pointer

    public:  /* Constructors & Destructor */
        TlsClient() = delete;
        TlsClient(const TlsClient&) = delete;
        TlsClient(TlsClient&& t_client) noexcept;

        TlsClient(io_context_t& t_io_ctx,
                  shared_ptr<Args> t_argsp,
                  shared_ptr<TextRc> t_rcp);

        virtual ~TlsClient();

    public:  /* Operators */
        TlsClient& operator=(const TlsClient&) = delete;
        TlsClient& operator=(TlsClient&& t_client) noexcept;

    public:  /* Methods */
        /**
        * @brief
        *     Get the remote host state based on the last socket error code.
        */
        constexpr HostState host_state() const noexcept override
        {
            HostState state{HostState::closed};
            const bool truncated{m_ecode == ssl::error::stream_truncated};

            if (connect_timed_out(m_ecode))
            {
                state = HostState::unknown;
            }
            else if (net::no_error(m_ecode) || truncated || operation_timed_out(m_ecode))
            {
                state = HostState::open;
            }
            return state;
        }

        void close() override;
        void connect(const Endpoint& t_ep) override;
        void connect(port_t t_port) override;

        size_t recv(buffer_t& t_buffer) override;
        size_t send(const string& t_payload) override;

        Response<> request(const Request<>& t_request) override;
        Response<> request(const string& t_host, const string& t_uri = URI_ROOT) override;

        Response<> request(verb_t t_method,
                           const string& t_host,
                           const string& t_uri = URI_ROOT,
                           const string& t_body = {}) override;

    private:  /* Methods */
        void async_handshake();
        void on_connect(const net_error_code_t& t_ecode, Endpoint t_ep) override;
        void on_handshake(const net_error_code_t& t_ecode);

        bool on_verify(bool t_preverified, verify_context_t& t_verify_ctx);
        bool valid_handshake() const;

        OSSL_HANDSHAKE_STATE handshake_state() const;

        const SSL_CIPHER* cipher_ptr() const;

        net_error_code_t handshake();

        string cipher_suite() const;

        const stream_t& stream() const noexcept override;
        stream_t& stream() noexcept override;

        const socket_t& socket() const noexcept override;
        socket_t& socket() noexcept override;
    };
}

#endif // !SCAN_TLS_CLIENT_H
