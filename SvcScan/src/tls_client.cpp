/*
*  tls_client.cpp
*  --------------
*  Source file for a secure IPv4 TCP socket client
*/
#include "includes/except/null_ptr_ex.h"
#include "includes/inet/sockets/tls_client.h"

/// ***
/// Initialize the object
/// ***
scan::TlsClient::TlsClient(TlsClient &&t_client) noexcept
    : base_t(std::forward<TlsClient>(t_client)) {

    m_ctxp = std::move(t_client.m_ctxp);
    m_ssl_streamp = std::move(t_client.m_ssl_streamp);
}

/// ***
/// Initialize the object
/// ***
scan::TlsClient::TlsClient(io_context &t_ioc, const Args &t_args)
    : base_t(t_ioc, t_args) {

    m_ctxp = std::make_unique<ctx_t>(ctx_t::tlsv12_client);
    m_ctxp->set_verify_mode(ssl::verify_none);

    m_ssl_streamp = std::make_unique<ssl_stream_t>(m_ioc, *m_ctxp);
}

/// ***
/// Destroy the object
/// ***
scan::TlsClient::~TlsClient()
{
    if (is_open())
    {
        error_code discard_ecode;
        socket().close(discard_ecode);
    }
}

/// ***
/// Determine whether the SSL/TLS handshake state is valid for the underlying socket
/// ***
bool scan::TlsClient::valid_handshake() const
{
    const OSSL_HANDSHAKE_STATE hs_state{ handshake_state() };

    const bool successful{ hs_state == OSSL_HANDSHAKE_STATE::TLS_ST_OK };
    const bool not_attempted{ hs_state == OSSL_HANDSHAKE_STATE::TLS_ST_BEFORE };

    return successful || not_attempted;
}

/// ***
/// Get the remote host state based on the underlying socket error code
/// ***
scan::HostState scan::TlsClient::host_state() const noexcept
{
    return host_state(m_ecode);
}

/// ***
/// Get the remote host state based on the given socket error code
/// ***
scan::HostState scan::TlsClient::host_state(const error_code &t_ecode) const noexcept
{
    HostState state{ HostState::open };

    const bool timeout_error = t_ecode == error::timed_out
                            || t_ecode == beast_error::timeout;

    if (!m_connected && timeout_error)
    {
        state = HostState::unknown;
    }
    else if (!net::no_error(t_ecode) && t_ecode != ssl::error::stream_truncated)
    {
        state = HostState::closed;
    }
    return state;
}

/// ***
/// Get the OpenSSL handshake state of the underlying SSL/TLS stream
/// ***
OSSL_HANDSHAKE_STATE scan::TlsClient::handshake_state() const
{
    return SSL_get_state(connection_ptr());
}

/// ***
/// Read inbound data from the underlying SSL/TLS socket stream
/// ***
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

/// ***
/// Get a constant pointer to the underlying SSL/TLS connection
/// ***
const SSL *scan::TlsClient::connection_ptr() const noexcept
{
    const SSL *sslp{ nullptr };

    if (m_ssl_streamp != nullptr)
    {
        sslp = static_cast<const SSL *>(m_ssl_streamp->native_handle());
    }
    return sslp;
}

/// ***
/// Get a constant pointer to the underlying SSL/TLS connection cipher
/// ***
const SSL_CIPHER *scan::TlsClient::cipher_ptr() const
{
    const SSL_CIPHER *cipherp{ nullptr };

    if (connection_ptr() != nullptr)
    {
        cipherp = SSL_get_current_cipher(connection_ptr());
    }
    return cipherp;
}

/// ***
/// Get a constant pointer to the underlying SSL/TLS connection X509 certificate
/// ***
const X509 *scan::TlsClient::x509_ptr(verify_cxt_t &t_vctx) const
{
    const X509 *certp{ nullptr };

    if (x509_ctx_ptr(t_vctx) != nullptr)
    {
        certp = X509_STORE_CTX_get_current_cert(x509_ctx_ptr(t_vctx));
    }
    return certp;
}

/// ***
/// Get a constant pointer to the underlying SSL/TLS connection X509 store context
/// ***
const X509_STORE_CTX *scan::TlsClient::x509_ctx_ptr(verify_cxt_t &t_vctx) const
{
    return t_vctx.native_handle();
}

/// ***
/// Perform TLS handshake negotiations on the underlying SSL/TLS stream
/// ***
boost::system::error_code scan::TlsClient::handshake()
{
    m_ssl_streamp->handshake(ssl_stream_t::client, m_ecode);
    return m_ecode;
}

/// ***
/// Send the given string payload to the connected remote endpoint
/// ***
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

/// ***
/// Get a constant reference to the underlying SSL/TLS socket
/// ***
const scan::TlsClient::socket_t &scan::TlsClient::socket() const noexcept
{
    return stream().socket();
}

/// ***
/// Get a reference to the underlying SSL/TLS socket
/// ***
scan::TlsClient::socket_t &scan::TlsClient::socket() noexcept
{
    return stream().socket();
}

/// ***
/// Get a constant reference to the underlying SSL/TLS socket stream
/// ***
const scan::TlsClient::stream_t &scan::TlsClient::stream() const noexcept
{
    return boost::beast::get_lowest_layer(*m_ssl_streamp);
}

/// ***
/// Get a reference to the underlying SSL/TLS socket stream
/// ***
scan::TlsClient::stream_t &scan::TlsClient::stream() noexcept
{
    return boost::beast::get_lowest_layer(*m_ssl_streamp);
}

/// ***
/// Read all inbound data available from the underlying SSL/TLS socket stream
/// ***
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

/// ***
/// Send the given HTTPS request and return the server's response
/// ***
scan::Response<> scan::TlsClient::request(const Request<> &t_request)
{
    if (!t_request.valid())
    {
        throw ArgEx{ "t_request", "Invalid HTTP request" };
    }
    Response<> response;

    // Perform HTTPS communications
    if (connected_check())
    {
        http::write(*m_ssl_streamp, t_request.request(), m_ecode);

        if (success_check())
        {
            response_t resp;

            http::read(*m_ssl_streamp, response.buffer, resp, m_ecode);

            if (valid(m_ecode))
            {
                response.parse(resp);
            }
        }
    }
    return response;
}

/// ***
/// Send an HTTPS request and return the server's response
/// ***
scan::Response<> scan::TlsClient::request(const string &t_host, const string &t_uri)
{
    return request(verb_t::get, t_host, t_uri);
}

/// ***
/// Send an HTTPS request and return the server's response
/// ***
scan::Response<> scan::TlsClient::request(const verb_t &t_method,
                                          const string &t_host,
                                          const string &t_uri,
                                          const string &t_body) {
    Response<> response;

    if (connected_check())
    {
        response = request({ t_method, t_host, t_uri, t_body });
    }
    return response;
}

/// ***
/// Callback handler method for asynchronous connect operations
/// ***
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

    // Perform TLS handshake negotiations
    if (success_check())
    {
        handshake();
        m_connected = valid_handshake();

        if (m_connected && net::no_error(m_ecode) && m_verbose)
        {
            StdUtil::printf("Connection established: %/tcp", t_ep.port);
        }
    }
}
