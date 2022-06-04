/*
*  tls_client.cpp
*  --------------
*  Source file for a secure IPv4 TCP socket client
*/
#include <sdkddkver.h>
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

    m_ctxp = std::make_unique<context_t>(context_t::tlsv12_client);
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
/// Perform TLS handshake negotiations on the underlying TLS stream
/// ***
boost::system::error_code scan::TlsClient::handshake()
{
    m_ssl_streamp->handshake(ssl::stream_base::client, m_ecode);
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
/// Read inbound data from the underlying socket stream
/// ***
size_t scan::TlsClient::recv(char (&t_buffer)[BUFFER_SIZE],
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
        const asio::mutable_buffer mutable_buffer{ t_buffer, BUFFER_SIZE };

        bytes_read = m_ssl_streamp->read_some(mutable_buffer, t_ecode);
        m_ecode = t_ecode;
    }
    return bytes_read;
}

/// ***
/// Get a constant reference to the underlying TCP socket
/// ***
const scan::TlsClient::socket_t &scan::TlsClient::socket() const noexcept
{
    return stream().socket();
}

/// ***
/// Get a reference to the underlying TCP socket
/// ***
scan::TlsClient::socket_t &scan::TlsClient::socket() noexcept
{
    return stream().socket();
}

/// ***
/// Get a constant reference to the underlying TCP socket stream
/// ***
const scan::TlsClient::stream_t &scan::TlsClient::stream() const noexcept
{
    return boost::beast::get_lowest_layer(*m_ssl_streamp);
}

/// ***
/// Get a reference to the underlying TCP socket stream
/// ***
scan::TlsClient::stream_t &scan::TlsClient::stream() noexcept
{
    return boost::beast::get_lowest_layer(*m_ssl_streamp);
}

/// ***
/// Read all inbound data currently available in the underlying socket stream
/// ***
std::string scan::TlsClient::recv(error_code &t_ecode, const Timeout &t_timeout)
{
    std::stringstream data;

    size_t bytes_read{ 0 };
    char recv_buffer[BUFFER_SIZE]{ '\0' };

    do  // Read until EOF/error is detected
    {
        bytes_read = recv(recv_buffer, t_ecode, t_timeout);

        // All data received
        if (t_ecode == error::eof)
        {
            break;
        }

        // Copy buffer data
        if (valid(t_ecode))
        {
            data << std::string_view(recv_buffer, bytes_read);
        }
    }
    while (valid(t_ecode) && bytes_read > 0);

    return data.str();
}

/// ***
/// Send the given HTTP request and return the server's response
/// ***
scan::Response<> scan::TlsClient::request(const Request<> &t_request)
{
    if (!t_request.valid())
    {
        throw ArgEx{ "t_request", "Invalid HTTP request" };
    }
    Response<> response;

    // Perform HTTP communications
    if (connected_check())
    {
        http::write(*m_ssl_streamp, t_request.request(), m_ecode);

        if (success_check(m_ecode))
        {
            response_t resp;

            http::read(*m_ssl_streamp, response.buffer, resp, m_ecode);

            if (success_check(m_ecode))
            {
                response.parse(resp);
            }
        }
    }
    return response;
}

/// ***
/// Send an HTTP request and return the server's response
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
    if (success_check(m_ecode) && success_check(m_ecode = handshake()))
    {
        if (m_verbose)
        {
            StdUtil::printf("Connection established: %/tcp", t_ep.port);
        }
        m_connected = true;
    }
}
