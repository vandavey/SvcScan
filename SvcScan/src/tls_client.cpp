/*
*  tls_client.cpp
*  --------------
*  Source file for a secure IPv4 TCP socket client
*/
#include <sdkddkver.h>
#include <boost/asio/placeholders.hpp>
#include <boost/bind/bind.hpp>
#include "includes/except/null_ptr_ex.h"
#include "includes/inet/sockets/tls_client.h"

/**
* @brief  Initialize the object.
*/
scan::TlsClient::TlsClient(TlsClient &&t_client) noexcept
    : base_t(t_client.m_ioc, t_client.m_args_ap, t_client.m_trc_ap) {

    *this = std::forward<this_t>(t_client);
}

/**
* @brief  Initialize the object.
*/
scan::TlsClient::TlsClient(io_context &t_ioc,
                           shared_ptr<Args> t_argsp,
                           shared_ptr<TextRc> t_trcp)
    : base_t(t_ioc, t_argsp, t_trcp) {

    m_ctxp = std::make_unique<ctx_t>(ctx_t::tlsv12_client);

    m_ctxp->set_default_verify_paths(m_ecode);
    m_ctxp->set_verify_mode(ssl::verify_none);

    m_ssl_streamp = std::make_unique<ssl_stream_t>(m_ioc, *m_ctxp);
}

/**
* @brief  Destroy the object.
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
* @brief  Move assignment operator overload.
*/
scan::TlsClient &scan::TlsClient::operator=(TlsClient &&t_client) noexcept
{
    if (this != &t_client)
    {
        m_ctxp = std::move(t_client.m_ctxp);
        m_ssl_streamp = std::move(t_client.m_ssl_streamp);

        m_connected = t_client.m_connected;
        m_conn_timeout = t_client.m_conn_timeout;
        m_ecode = t_client.m_ecode;
        m_recv_timeout = t_client.m_recv_timeout;
        m_remote_ep = t_client.m_remote_ep;
        m_send_timeout = t_client.m_send_timeout;
        m_streamp = std::move(t_client.m_streamp);
        m_svc_info = t_client.m_svc_info;
        m_verbose = t_client.m_verbose;

        m_args_ap.store(t_client.m_args_ap);
        m_trc_ap.store(std::move(t_client.m_trc_ap));
    }
    return *this;
}

/**
* @brief  Asynchronously perform TLS handshake negotiations on the underlying
*         SSL/TLS stream. Does not wait for completion and returns immediately.
*/
void scan::TlsClient::async_handshake(const Timeout &t_timeout)
{
    const auto call_wrapper = boost::bind(&this_t::on_handshake,
                                          this,
                                          asio::placeholders::error);

    stream().expires_after(static_cast<chrono::milliseconds>(t_timeout));
    m_ssl_streamp->async_handshake(ssl_stream_t::client, call_wrapper);
}

/**
* @brief  Close the underlying TCP socket.
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
* @brief  Establish a network connection to the given TCP endpoint.
*/
void scan::TlsClient::connect(const Endpoint &t_ep)
{
    if (!net::valid_endpoint(m_remote_ep = t_ep))
    {
        throw ArgEx{ "t_ep", "Invalid IPv4 endpoint" };
    }

    m_svc_info.addr = t_ep.addr;
    m_svc_info.port = std::to_string(t_ep.port);

    // Perform DNS name resolution
    results_t results{ net::resolve(m_ioc, m_remote_ep, m_ecode) };

    // Establish the connection
    if (success_check())
    {
        async_connect(results, m_conn_timeout);
        await_task();

        // Perform TLS handshake negotiations
        if (success_check())
        {
            handshake();
            m_connected = valid_handshake();

            if (m_connected && net::no_error(m_ecode) && m_verbose)
            {
                StdUtil::printf("SSL/TLS connection established: %/tcp", t_ep.port);
            }
        }
    }
}

/**
* @brief  Establish a network connection to underlying target on the given port.
*/
void scan::TlsClient::connect(const uint &t_port)
{
    // Invalid network port
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number" };
    }

    // Unknown remote host address
    if (m_remote_ep.addr.empty() || m_remote_ep.addr == Endpoint::IPV4_ANY)
    {
        if (m_args_ap.load()->target.addr().empty())
        {
            throw RuntimeEx{ "TlsClient::connect", "Invalid underlying target" };
        }
        m_remote_ep = { m_args_ap.load()->target.addr(), t_port };
    }
    connect(m_remote_ep);
}

/**
* @brief  Determine whether the SSL/TLS handshake state is currently
*         valid for the TCP underlying socket.
*/
bool scan::TlsClient::valid_handshake() const
{
    const OSSL_HANDSHAKE_STATE hs_state{ handshake_state() };

    const bool successful{ hs_state == OSSL_HANDSHAKE_STATE::TLS_ST_OK };
    const bool not_attempted{ hs_state == OSSL_HANDSHAKE_STATE::TLS_ST_BEFORE };

    return successful || not_attempted;
}

/**
* @brief  Get the remote host state based on the last socket error code.
*/
scan::HostState scan::TlsClient::host_state() const noexcept
{
    return host_state(m_ecode);
}

/**
* @brief  Get the remote host state based on the given socket error code.
*/
scan::HostState scan::TlsClient::host_state(const error_code &t_ecode) const noexcept
{
    HostState state{ HostState::closed };
    const bool truncated{ t_ecode == ssl::error::stream_truncated };

    const bool timeout_error = t_ecode == error::timed_out
                            || t_ecode == beast_error::timeout;

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
* @brief  Get the OpenSSL handshake state from the underlying SSL/TLS stream.
*/
OSSL_HANDSHAKE_STATE scan::TlsClient::handshake_state() const
{
    return SSL_get_state(connection_ptr());
}

/**
* @brief  Read inbound data from the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::recv(char (&t_buffer)[BUFFER_SIZE])
{
    return recv(t_buffer, m_ecode, m_recv_timeout);
}

/**
* @brief  Read inbound data from the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::recv(char (&t_buffer)[BUFFER_SIZE], error_code &t_ecode)
{
    return recv(t_buffer, t_ecode, m_recv_timeout);
}

/**
* @brief  Read inbound data from the underlying SSL/TLS socket stream.
*/
size_t scan::TlsClient::recv(char (&t_buffer)[BUFFER_SIZE],
                             error_code &t_ecode,
                             const Timeout &t_timeout) {
    if (t_buffer == nullptr)
    {
        throw NullArgEx{ "t_buffer" };
    }

    string data;
    size_t bytes_read{ 0U };

    // Read inbound stream data
    if (connected_check())
    {
        recv_timeout(t_timeout);
        const asio::mutable_buffer mutable_buffer{ t_buffer, BUFFER_SIZE };

        bytes_read = m_ssl_streamp->read_some(mutable_buffer, t_ecode);
        m_ecode = t_ecode;
    }
    return bytes_read;
}

/**
* @brief  Get a constant pointer to the underlying SSL/TLS connection.
*/
SSL *scan::TlsClient::connection_ptr() const noexcept
{
    SSL *sslp{ nullptr };

    if (m_ssl_streamp != nullptr)
    {
        sslp = m_ssl_streamp->native_handle();
    }
    return sslp;
}

/**
* @brief  Get a constant pointer to the underlying SSL/TLS connection cipher.
*/
const SSL_CIPHER *scan::TlsClient::cipher_ptr() const
{
    const SSL_CIPHER *cipherp{ nullptr };

    if (connection_ptr() != nullptr)
    {
        cipherp = SSL_get_current_cipher(connection_ptr());
    }
    return cipherp;
}

/**
* @brief  Get a constant pointer to the SSL/TLS connection X509 certificate.
*/
X509 *scan::TlsClient::x509_ptr(verify_cxt_t &t_vctx) const
{
    X509 *certp{ nullptr };

    if (x509_ctx_ptr(t_vctx) != nullptr)
    {
        certp = X509_STORE_CTX_get_current_cert(x509_ctx_ptr(t_vctx));
    }
    return certp;
}

/**
* @brief  Get a constant pointer to the SSL/TLS connection X509 store context.
*/
X509_STORE_CTX *scan::TlsClient::x509_ctx_ptr(verify_cxt_t &t_vctx) const
{
    return t_vctx.native_handle();
}

/**
* @brief  Get a constant reference to the underlying TCP socket stream.
*/
const scan::TlsClient::stream_t &scan::TlsClient::stream() const noexcept
{
    return boost::beast::get_lowest_layer(*m_ssl_streamp);
}

/**
* @brief  Get a reference to the underlying TCP socket stream.
*/
scan::TlsClient::stream_t &scan::TlsClient::stream() noexcept
{
    return boost::beast::get_lowest_layer(*m_ssl_streamp);
}

/**
* @brief  Perform TLS handshake negotiations on the underlying SSL/TLS stream.
*/
boost::system::error_code scan::TlsClient::handshake()
{
    async_handshake();
    await_task();

    if (!net::no_error(m_ecode))
    {
        stream().cancel();
    }
    return m_ecode;
}

/**
* @brief  Write the given string payload to the underlying SSL/TLS socket stream.
*/
boost::system::error_code scan::TlsClient::send(const string &t_payload)
{
    return send(t_payload, m_send_timeout);
}

/**
* @brief  Write the given string payload to the underlying SSL/TLS socket stream.
*/
boost::system::error_code scan::TlsClient::send(const string &t_payload,
                                                const Timeout &t_timeout) {
    if (connected_check())
    {
        send_timeout(t_timeout);

        // Send the payload
        if (connected_check() && !t_payload.empty())
        {
            m_ssl_streamp->write_some(asio::buffer(t_payload), m_ecode);
        }
    }
    return m_ecode;
}

/**
* @brief  Get a constant reference to the underlying SSL/TLS socket.
*/
const scan::TlsClient::socket_t &scan::TlsClient::socket() const noexcept
{
    return stream().socket();
}

/**
* @brief  Get a reference to the underlying SSL/TLS socket.
*/
scan::TlsClient::socket_t &scan::TlsClient::socket() noexcept
{
    return stream().socket();
}

/**
* @brief  Read all inbound data available from the underlying SSL/TLS socket stream.
*/
std::string scan::TlsClient::recv()
{
    return recv(m_ecode, m_recv_timeout);
}

/**
* @brief  Read all inbound data available from the underlying SSL/TLS socket stream.
*/
std::string scan::TlsClient::recv(error_code &t_ecode)
{
    return recv(t_ecode, m_recv_timeout);
}

/**
* @brief  Read all inbound data available from the underlying SSL/TLS socket stream.
*/
std::string scan::TlsClient::recv(error_code &t_ecode, const Timeout &t_timeout)
{
    std::stringstream data;

    size_t bytes_read{ 0 };
    char recv_buffer[BUFFER_SIZE]{ '\0' };

    do  // Read until EOF/error is detected
    {
        bytes_read = recv(recv_buffer, t_ecode, t_timeout);

        if (t_ecode == error::eof)
        {
            break;
        }

        if (valid(t_ecode))
        {
            data << std::string_view(recv_buffer, bytes_read);
        }
    }
    while (valid(t_ecode) && bytes_read > 0);

    return data.str();
}

/**
* @brief  Send the given HTTPS request and return the server's response.
*/
scan::Response<> scan::TlsClient::request(const Request<> &t_request)
{
    if (!t_request.valid())
    {
        throw ArgEx{ "t_request", "Invalid HTTP request" };
    }
    Response response;

    // Perform HTTPS communications
    if (connected_check())
    {
        http::write(*m_ssl_streamp, t_request.message(), m_ecode);

        if (success_check())
        {
            const string raw_resp{ recv() };

            if (!raw_resp.empty())
            {
                response.parse(raw_resp);
            }
        }
    }
    return response;
}

/**
* @brief  Send an HTTPS request and return the server's response.
*/
scan::Response<> scan::TlsClient::request(const string &t_host, const string &t_uri)
{
    return request(verb_t::get, t_host, t_uri);
}

/**
* @brief  Send an HTTPS request and return the server's response.
*/
scan::Response<> scan::TlsClient::request(const verb_t &t_method,
                                          const string &t_host,
                                          const string &t_uri,
                                          const string &t_body) {
    Response response;

    if (connected_check())
    {
        response = request({ t_method, t_host, t_uri, t_body });
    }
    return response;
}

/**
* @brief  Callback handler for asynchronous connect operations.
*/
void scan::TlsClient::on_connect(const error_code &t_ecode, Endpoint t_ep)
{
    m_ecode = t_ecode;

    // Ensure accuracy of socket error and host state
    if (m_ecode == error::host_not_found)
    {
        m_svc_info.state = HostState::closed;
        m_ecode = error::connection_refused;
    }
    else if (!net::no_error(m_ecode))
    {
        m_svc_info.state = HostState::unknown;
    }

    m_connected = success_check();
}

/**
* @brief  Callback handler for asynchronous SSL/TLS handshake operations.
*/
void scan::TlsClient::on_handshake(const error_code &t_ecode)
{
    m_ecode = t_ecode;
    m_connected = net::no_error(m_ecode) && valid_handshake();
}

/**
* @brief  Determine whether the given error indicates a successful operation.
*/
bool scan::TlsClient::valid(const error_code &t_ecode,
                            const bool &t_eof_valid) noexcept {

    bool no_error{ net::no_error(t_ecode) };
    const bool truncated{ t_ecode == ssl::error::stream_truncated };

    if (t_eof_valid)
    {
        no_error = no_error || truncated || t_ecode == error::eof;
    }
    return no_error;
}
