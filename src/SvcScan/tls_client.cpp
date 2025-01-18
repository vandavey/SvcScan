/*
* @file
*     tls_client.cpp
* @brief
*     Source file for a secure IPv4 TCP socket client.
*/
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <boost/beast/core/error.hpp>
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
#include "includes/inet/net.h"
#include "includes/inet/sockets/tls_client.h"
#include "includes/ranges/algo.h"
#include "includes/utils/const_defs.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Initialize the object.
*/
scan::TlsClient::TlsClient(TlsClient&& t_client) noexcept
    : TcpClient{t_client.m_ioc, t_client.m_args_ap, t_client.m_trc_ap}
{
    *this = std::move(t_client);
}

/**
* @brief
*     Initialize the object.
*/
scan::TlsClient::TlsClient(io_context& t_ioc,
                           shared_ptr<Args> t_argsp,
                           shared_ptr<TextRc> t_trcp)
    : TcpClient{t_ioc, t_argsp, t_trcp}
{
    m_ctxp = std::make_unique<ssl_context>(ssl_context::tlsv12_client);

    auto verify_callback = std::bind(&TlsClient::on_verify,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2);

    m_ctxp->set_default_verify_paths(m_ecode);
    m_ctxp->set_verify_mode(ssl::verify_none);
    m_ctxp->set_verify_callback(std::move(verify_callback), m_ecode);

    m_ssl_streamp = std::make_unique<ssl_stream_t>(m_ioc, *m_ctxp);
}

/**
* @brief
*     Destroy the object.
*/
scan::TlsClient::~TlsClient()
{
    if (is_open())
    {
        error_code discard_ecode;
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
        m_ctxp = std::move(t_client.m_ctxp);
        m_ssl_streamp = std::move(t_client.m_ssl_streamp);
        TcpClient::operator=(std::move(t_client));
    }
    return *this;
}

/**
* @brief
*     Asynchronously perform TLS handshake negotiations on the underlying
*     SSL/TLS stream. Does not wait for completion and returns immediately.
*/
void scan::TlsClient::async_handshake(const Timeout& t_timeout)
{
    auto handshake_callback = boost::bind(&TlsClient::on_handshake,
                                          this,
                                          asio::placeholders::error);

    stream().expires_after(static_cast<milliseconds>(t_timeout));
    m_ssl_streamp->async_handshake(ssl_stream_t::client, std::move(handshake_callback));
}

/**
* @brief
*     Close the underlying TCP socket.
*/
void scan::TlsClient::close()
{
    if (is_open())
    {
        error_code ecode;
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
    results_t results{net::resolve(m_ioc, m_remote_ep, m_ecode)};

    // Establish the connection
    if (success_check())
    {
        async_connect(results, m_conn_timeout);
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
*     Determine whether the SSL/TLS handshake state
*     is currently valid for the underlying socket.
*/
bool scan::TlsClient::valid_handshake() const
{
    return algo::any_equal(handshake_state(), ::TLS_ST_BEFORE, ::TLS_ST_OK);
}

/**
* @brief
*     Get the remote host state based on the last socket error code.
*/
scan::HostState scan::TlsClient::host_state() const noexcept
{
    return host_state(m_ecode);
}

/**
* @brief
*     Get the remote host state based on the given socket error code.
*/
scan::HostState scan::TlsClient::host_state(const error_code& t_ecode) const noexcept
{
    HostState state{HostState::closed};
    const bool truncated{t_ecode == ssl::error::stream_truncated};

    const bool timeout_error = t_ecode == asio::error::timed_out
                            || t_ecode == beast::error::timeout;

    if (!m_connected && timeout_error)
    {
        state = HostState::unknown;
    }
    else if (net::no_error(t_ecode) || truncated || (m_connected && timeout_error))
    {
        state = HostState::open;
    }
    return state;
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
*     Read inbound data from the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::recv(buffer_t& t_buffer)
{
    return recv(t_buffer, m_ecode, m_recv_timeout);
}

/**
* @brief
*     Read inbound data from the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::recv(buffer_t& t_buffer, error_code& t_ecode)
{
    return recv(t_buffer, t_ecode, m_recv_timeout);
}

/**
* @brief
*     Read inbound data from the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::recv(buffer_t& t_buffer,
                             error_code& t_ecode,
                             const Timeout& t_timeout)
{
    string data;
    size_t num_read{0_st};

    // Read inbound stream data
    if (connected_check())
    {
        recv_timeout(t_timeout);
        const asio::mutable_buffer mutable_buffer{&t_buffer[0], sizeof t_buffer};

        num_read = m_ssl_streamp->read_some(mutable_buffer, t_ecode);
        m_ecode = t_ecode;
    }
    return num_read;
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
*     Perform TLS handshake negotiations on the underlying SSL/TLS stream.
*/
scan::error_code scan::TlsClient::handshake()
{
    async_handshake();
    async_await();

    if (!net::no_error(m_ecode))
    {
        stream().cancel();
    }
    return m_ecode;
}

/**
* @brief
*     Write the given string payload to the underlying SSL/TLS socket stream.
*/
scan::error_code scan::TlsClient::send(const string& t_payload)
{
    return send(t_payload, m_send_timeout);
}

/**
* @brief
*     Write the given string payload to the underlying SSL/TLS socket stream.
*/
scan::error_code scan::TlsClient::send(const string& t_payload,
                                       const Timeout& t_timeout)
{
    if (connected_check())
    {
        send_timeout(t_timeout);

        if (connected_check() && !t_payload.empty())
        {
            m_ssl_streamp->write_some(asio::buffer(t_payload), m_ecode);
        }
    }
    return m_ecode;
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
*     Read all inbound data available from the underlying SSL/TLS socket stream.
*/
std::string scan::TlsClient::recv()
{
    return recv(m_ecode, m_recv_timeout);
}

/**
* @brief
*     Read all inbound data available from the underlying SSL/TLS socket stream.
*/
std::string scan::TlsClient::recv(error_code& t_ecode)
{
    return recv(t_ecode, m_recv_timeout);
}

/**
* @brief
*     Read all inbound data available from the underlying SSL/TLS socket stream.
*/
std::string scan::TlsClient::recv(error_code& t_ecode, const Timeout& t_timeout)
{
    bool no_error;
    sstream stream;

    size_t num_read{0_st};
    buffer_t recv_buffer{CHAR_NULL};

    do  // Read until EOF or error is detected
    {
        num_read = recv(recv_buffer, t_ecode, t_timeout);

        if (no_error = valid(t_ecode, false) && num_read > 0)
        {
            stream << string(&recv_buffer[0], num_read);
        }
    }
    while (no_error);

    return stream.str();
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
            http::response_parser<string_body> parser;
            beast::flat_buffer& buffer{response.buffer};

            size_t num_read{http::read_header(*m_ssl_streamp, buffer, parser, m_ecode)};

            if (m_ecode != http::error::bad_version && success_check(true, true))
            {
                do  // Read until end reached or message fully parsed
                {
                    num_read = http::read(*m_ssl_streamp, buffer, parser, m_ecode);
                }
                while (num_read > 0 && net::no_error(m_ecode));

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
        response = request({t_method, t_host, t_uri, t_body});
    }
    return response;
}

/**
* @brief
*     Callback handler for asynchronous connect operations.
*/
void scan::TlsClient::on_connect(const error_code& t_ecode, Endpoint t_ep)
{
    m_ecode = t_ecode;

    // Ensure accuracy of socket error and host state
    if (m_ecode == asio::error::host_not_found)
    {
        m_ecode = asio::error::connection_refused;
        m_svc_info.state(HostState::closed);
    }
    else if (!net::no_error(m_ecode))
    {
        m_svc_info.state(HostState::unknown);
    }

    m_connected = success_check();
}

/**
* @brief
*     Callback handler for asynchronous SSL/TLS handshake operations.
*/
void scan::TlsClient::on_handshake(const error_code& t_ecode)
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
bool scan::TlsClient::on_verify(bool t_preverified, verify_context& t_verify_ctx)
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
*     Determine whether the given error indicates a successful operation.
*/
bool scan::TlsClient::valid(const error_code& t_ecode,
                            bool t_allow_eof,
                            bool t_allow_partial)
    noexcept
{
    bool no_error{net::no_error(t_ecode)};

    if (!no_error && t_allow_eof)
    {
        no_error = t_ecode == asio::error::eof
                || t_ecode == http::error::end_of_stream
                || t_ecode == ssl::error::stream_truncated;
    }

    if (!no_error && t_allow_partial)
    {
        no_error = t_ecode == http::error::partial_message;
    }
    return no_error;
}
