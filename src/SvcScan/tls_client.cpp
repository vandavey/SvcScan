/*
* @file
*     tls_client.cpp
* @brief
*     Source file for a secure IPv4 TCP socket client.
*/
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http/error.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/bind/bind.hpp>
#include <openssl/types.h>
#include <openssl/x509_vfy.h>
#include "includes/console/util.h"
#include "includes/errors/arg_ex.h"
#include "includes/errors/error_const_defs.h"
#include "includes/errors/runtime_ex.h"
#include "includes/inet/sockets/tls_client.h"
#include "includes/ranges/algo.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Initialize the object.
*/
scan::TlsClient::TlsClient(TlsClient&& t_client) noexcept
    : TcpClient{t_client.m_io_ctx, t_client.m_args_ap, t_client.m_rc_ap}
{
    *this = std::move(t_client);
}

/**
* @brief
*     Initialize the object.
*/
scan::TlsClient::TlsClient(io_context_t& t_io_ctx,
                           shared_ptr<Args> t_argsp,
                           shared_ptr<TextRc> t_rcp)
    : TcpClient{t_io_ctx, t_argsp, t_rcp}
{
    m_ssl_ctxp = std::make_unique<ssl_context_t>(ssl_context_t::tlsv12_client);

    auto verify_callback = std::bind(&TlsClient::on_verify,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2);

    m_ssl_ctxp->set_default_verify_paths(m_ecode);
    m_ssl_ctxp->set_verify_mode(ssl::verify_none);
    m_ssl_ctxp->set_verify_callback(std::move(verify_callback), m_ecode);

    m_ssl_streamp = std::make_unique<ssl_stream_t>(m_io_ctx, *m_ssl_ctxp);
}

/**
* @brief
*     Destroy the object.
*/
scan::TlsClient::~TlsClient()
{
    if (is_open())
    {
        net_error_code_t discard_ecode;
        socket().close(discard_ecode);
    }
}

/**
* @brief
*     Move assignment operator overload.
*/
scan::TlsClient& scan::TlsClient::operator=(TlsClient&& t_client) noexcept
{
    if (this != &t_client)
    {
        m_ssl_ctxp = std::move(t_client.m_ssl_ctxp);
        m_ssl_streamp = std::move(t_client.m_ssl_streamp);
        TcpClient::operator=(std::move(t_client));
    }
    return *this;
}

/**
* @brief
*     Close the underlying TCP socket.
*/
void scan::TlsClient::close()
{
    if (is_open())
    {
        net_error_code_t ecode;
        socket().close(ecode);

        success_check(ecode);
        m_svc_info.reset(m_remote_ep.addr);
    }
}

/**
* @brief
*     Establish a network connection to the given TCP endpoint.
*/
void scan::TlsClient::connect(const Endpoint& t_ep)
{
    if (!net::valid_endpoint(m_remote_ep = t_ep))
    {
        throw ArgEx{INVALID_ENDPOINT_MSG, "t_ep"};
    }

    m_svc_info.addr = t_ep.addr;
    m_svc_info.port(t_ep.port);

    // Perform DNS name resolution
    results_t results{net::resolve(m_io_ctx, m_remote_ep, m_ecode)};

    // Establish the connection
    if (success_check())
    {
        async_connect(results);
        async_await();

        // Perform TLS handshake negotiations
        if (success_check())
        {
            handshake();
            m_connected = valid_handshake();

            if (m_connected && net::no_error(m_ecode) && m_verbose)
            {
                util::printf("SSL/TLS connection established: %/%", t_ep.port, PROTO);
            }
        }
    }
}

/**
* @brief
*     Establish a network connection to underlying target on the given port.
*/
void scan::TlsClient::connect(port_t t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{INVALID_PORTS_MSG, "t_port"};
    }

    // Unknown remote host address
    if (m_remote_ep.addr.empty() || m_remote_ep.addr == IPV4_ANY)
    {
        if (m_args_ap.load()->target.addr().empty())
        {
            throw RuntimeEx{INVALID_TARGET_MSG, "TlsClient::connect"};
        }
        m_remote_ep = {m_args_ap.load()->target.addr(), t_port};
    }
    connect(m_remote_ep);
}

/**
* @brief
*     Read inbound data from the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::recv(buffer_t& t_buffer)
{
    string data;
    size_t bytes_read{0_sz};

    if (connected_check())
    {
        recv_timeout(RECV_TIMEOUT);

        const mutable_buffer_t mut_buffer{&t_buffer[0], sizeof t_buffer};
        bytes_read = m_ssl_streamp->read_some(mut_buffer, m_ecode);
    }
    return bytes_read;
}

/**
* @brief
*     Write the given string payload to the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::send(const string& t_payload)
{
    size_t bytes_sent{0_sz};

    if (connected_check())
    {
        send_timeout(SEND_TIMEOUT);

        if (connected_check() && !t_payload.empty())
        {
            bytes_sent = m_ssl_streamp->write_some(asio::buffer(t_payload), m_ecode);
        }
    }
    return bytes_sent;
}

/**
* @brief
*     Send the given HTTPS request and return the server's response.
*/
scan::Response<> scan::TlsClient::request(const Request<>& t_request)
{
    if (!t_request.valid())
    {
        throw ArgEx{INVALID_REQUEST_MSG, "t_request"};
    }
    Response response;

    // Perform HTTPS communications
    if (connected_check())
    {
        http::write(*m_ssl_streamp, t_request.message(), m_ecode);

        if (success_check())
        {
            http::response_parser<string_body_t> parser;
            flat_buffer_t& buffer{response.buffer};

            size_t bytes_read{http::read_header(*m_ssl_streamp, buffer, parser, m_ecode)};

            if (m_ecode != http::error::bad_version && success_check(true, true))
            {
                do  // Read until end reached or message fully parsed
                {
                    bytes_read = http::read(*m_ssl_streamp, buffer, parser, m_ecode);
                }
                while (bytes_read > 0 && net::no_error(m_ecode));

                response.parse(parser.get());
            }
        }
    }
    return response;
}

/**
* @brief
*     Send an HTTPS request and return the server's response.
*/
scan::Response<> scan::TlsClient::request(const string& t_host, const string& t_uri)
{
    return request(verb_t::get, t_host, t_uri);
}

/**
* @brief
*     Send an HTTPS request and return the server's response.
*/
scan::Response<> scan::TlsClient::request(verb_t t_method,
                                          const string& t_host,
                                          const string& t_uri,
                                          const string& t_body)
{
    Response response;

    if (connected_check())
    {
        response = request(Request{t_method, t_host, t_uri, t_body});
    }
    return response;
}

/**
* @brief
*     Asynchronously perform TLS handshake negotiations on the underlying
*     SSL/TLS stream. Does not wait for completion and returns immediately.
*/
void scan::TlsClient::async_handshake()
{
    auto handshake_callback = boost::bind(&TlsClient::on_handshake,
                                          this,
                                          asio::placeholders::error);

    stream().expires_after(RECV_TIMEOUT);
    m_ssl_streamp->async_handshake(ssl_stream_t::client, std::move(handshake_callback));
}

/**
* @brief
*     Callback handler for asynchronous connect operations.
*/
void scan::TlsClient::on_connect(const net_error_code_t& t_ecode, Endpoint t_ep)
{
    m_ecode = t_ecode;

    // Ensure accuracy of socket error and host state
    if (m_ecode == asio::error::host_not_found)
    {
        m_ecode = asio::error::connection_refused;
        m_svc_info.state(HostState::closed);
    }
    else if (net::is_error(m_ecode))
    {
        m_svc_info.state(HostState::unknown);
    }

    m_connected = success_check();
}

/**
* @brief
*     Callback handler for asynchronous SSL/TLS handshake operations.
*/
void scan::TlsClient::on_handshake(const net_error_code_t& t_ecode)
{
    m_ecode = t_ecode;
    m_connected = net::no_error(m_ecode) && valid_handshake();

    if (m_connected)
    {
        m_svc_info.cipher = cipher_suite();
    }
}

/**
* @brief
*     Callback handler for SSL/TLS peer verification operations.
*/
bool scan::TlsClient::on_verify(bool t_preverified, verify_context_t& t_verify_ctx)
{
    if (t_preverified)
    {
        X509_STORE_CTX* ctxp{t_verify_ctx.native_handle()};

        if (ctxp != nullptr)
        {
            X509* certp{X509_STORE_CTX_get_current_cert(ctxp)};

            m_svc_info.issuer = net::x509_issuer(certp);
            m_svc_info.subject = net::x509_subject(certp);
        }
    }
    return true;
}

/**
* @brief
*     Determine whether the SSL/TLS handshake state
*     is currently valid for the underlying socket.
*/
bool scan::TlsClient::valid_handshake() const
{
    return algo::any_eq(handshake_state(), ::TLS_ST_BEFORE, ::TLS_ST_OK);
}

/**
* @brief
*     Get the OpenSSL handshake state from the underlying SSL/TLS stream.
*/
OSSL_HANDSHAKE_STATE scan::TlsClient::handshake_state() const
{
    if (m_ssl_streamp == nullptr)
    {
        throw RuntimeEx{NULL_PTR_DEREF_MSG, "TlsClient::handshake_state"};
    }
    return SSL_get_state(m_ssl_streamp->native_handle());
}

/**
* @brief
*     Get a constant pointer to the underlying SSL/TLS connection cipher.
*/
const SSL_CIPHER* scan::TlsClient::cipher_ptr() const
{
    const SSL_CIPHER* cipherp{nullptr};

    if (m_ssl_streamp != nullptr)
    {
        cipherp = SSL_get_current_cipher(m_ssl_streamp->native_handle());
    }
    return cipherp;
}

/**
* @brief
*     Perform TLS handshake negotiations on the underlying SSL/TLS stream.
*/
scan::net_error_code_t scan::TlsClient::handshake()
{
    async_handshake();
    async_await();

    if (net::is_error(m_ecode))
    {
        stream().cancel();
    }
    return m_ecode;
}

/**
* @brief
*     Get the cipher suite currently in use by the underlying SSL/TLS socket.
*/
std::string scan::TlsClient::cipher_suite() const
{
    string suite;
    const SSL_CIPHER* cipherp{cipher_ptr()};

    if (cipherp != nullptr)
    {
        suite = SSL_CIPHER_standard_name(cipherp);
    }
    return suite;
}

/**
* @brief
*     Get a constant reference to the underlying TCP socket stream.
*/
const scan::stream_t& scan::TlsClient::stream() const noexcept
{
    return beast::get_lowest_layer(*m_ssl_streamp);
}

/**
* @brief
*     Get a reference to the underlying TCP socket stream.
*/
scan::stream_t& scan::TlsClient::stream() noexcept
{
    return beast::get_lowest_layer(*m_ssl_streamp);
}

/**
* @brief
*     Get a constant reference to the underlying SSL/TLS socket.
*/
const scan::socket_t& scan::TlsClient::socket() const noexcept
{
    return stream().socket();
}

/**
* @brief
*     Get a reference to the underlying SSL/TLS socket.
*/
scan::socket_t& scan::TlsClient::socket() noexcept
{
    return stream().socket();
}
