/*
*  tcp_client.cpp
*  --------------
*  Source file for an IPv4 TCP socket client
*/
#include <sdkddkver.h>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/bind/bind.hpp>
#include "includes/inet/sockets/tcp_client.h"

/**
* @brief  Initialize the object.
*/
scan::TcpClient::TcpClient(TcpClient &&t_client) noexcept : m_ioc(t_client.m_ioc)
{
    *this = std::forward<this_t>(t_client);
}

/**
* @brief  Initialize the object.
*/
scan::TcpClient::TcpClient(io_context &t_ioc,
                           shared_ptr<Args> t_argsp,
                           shared_ptr<TextRc> t_trcp) : m_ioc(t_ioc) {
    m_connected = false;
    m_verbose = false;

    m_recv_timeout = RECV_TIMEOUT;
    m_send_timeout = SEND_TIMEOUT;

    m_trc_ap = t_trcp;
    m_streamp = std::make_unique<stream_t>(m_ioc);

    parse_argsp(t_argsp);
}

/**
* @brief  Destroy the object.
*/
scan::TcpClient::~TcpClient()
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
scan::TcpClient &scan::TcpClient::operator=(TcpClient &&t_client) noexcept
{
    if (this != &t_client)
    {
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
* @brief  Asynchronously establish a network connection on the underlying
*         TCP socket. Does not wait for completion and returns immediately.
*/
void scan::TcpClient::async_connect(const results_t &t_results,
                                    const Timeout &t_timeout) {

    auto call_wrapper = boost::bind(&this_t::on_connect,
                                    this,
                                    asio::placeholders::error,
                                    asio::placeholders::endpoint);

    stream().expires_after(static_cast<chrono::milliseconds>(t_timeout));
    stream().async_connect(t_results, std::move(call_wrapper));
}

/**
* @brief  Await the completion of the most recent asynchronous operation.
*/
void scan::TcpClient::await_task()
{
    m_ioc.run();
    m_ioc.restart();
}

/**
* @brief  Close the underlying TCP socket.
*/
void scan::TcpClient::close()
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
void scan::TcpClient::connect(const Endpoint &t_ep)
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
    }
}

/**
* @brief  Establish a network connection to underlying target on the given port.
*/
void scan::TcpClient::connect(const uint &t_port)
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
            throw RuntimeEx{ "TcpClient::connect", "Invalid underlying target" };
        }
        m_remote_ep = { m_args_ap.load()->target.addr(), t_port };
    }
    connect(m_remote_ep);
}

/**
* @brief  Set the socket connection timeout duration.
*/
void scan::TcpClient::connect_timeout(const Timeout &t_timeout)
{
    m_conn_timeout = t_timeout;
}

/**
* @brief  Disconnect from the remote host and close the underlying TCP socket.
*/
void scan::TcpClient::disconnect()
{
    if (connected_check())
    {
        shutdown();
    }
    m_connected = false;
}

/**
* @brief  Parse information from the given command-line arguments smart pointer.
*/
void scan::TcpClient::parse_argsp(shared_ptr<Args> t_argsp)
{
    m_args_ap = t_argsp;
    m_svc_info.addr = t_argsp->target;
    m_conn_timeout = t_argsp->timeout;
    m_verbose = t_argsp->verbose;
}

/**
* @brief  Set the timeout for synchronous socket receive operations.
*/
void scan::TcpClient::recv_timeout(const Timeout &t_timeout)
{
    if (m_recv_timeout != t_timeout)
    {
        m_recv_timeout = t_timeout;
    }
    set_timeout<SO_RCVTIMEO>(m_recv_timeout);
}

/**
* @brief  Set the timeout for synchronous socket send operations.
*/
void scan::TcpClient::send_timeout(const Timeout &t_timeout)
{
    if (m_send_timeout != t_timeout)
    {
        m_send_timeout = t_timeout;
    }
    set_timeout<SO_SNDTIMEO>(m_send_timeout);
}

/**
* @brief  Shutdown further communications on the underlying TCP socket.
*/
void scan::TcpClient::shutdown()
{
    if (connected_check() && is_open())
    {
        socket().shutdown(socket_t::shutdown_both, m_ecode);
        success_check();
    }
}

/**
* @brief  Determine whether the underlying TCP socket is connected.
*/
bool scan::TcpClient::is_connected() const noexcept
{
    return m_connected;
}

/**
* @brief  Determine whether the underlying TCP socket is open.
*/
bool scan::TcpClient::is_open() const noexcept
{
    return socket().is_open();
}

/**
* @brief  Get the remote host state based on the last socket error code.
*/
scan::HostState scan::TcpClient::host_state() const noexcept
{
    return host_state(m_ecode);
}

/**
* @brief  Get the remote host state based on the given socket error code.
*/
scan::HostState scan::TcpClient::host_state(const error_code &t_ecode) const noexcept
{
    HostState state{ HostState::closed };

    const bool timeout_error = t_ecode == error::timed_out
                            || t_ecode == beast_error::timeout;

    if (!m_connected && timeout_error)
    {
        state = HostState::unknown;
    }
    else if (net::no_error(t_ecode) || (m_connected && timeout_error))
    {
        state = HostState::open;
    }
    return state;
}

/**
* @brief  Read inbound data from the underlying socket stream.
*/
size_t scan::TcpClient::recv(char (&t_buffer)[BUFFER_SIZE])
{
    return recv(t_buffer, m_ecode, m_recv_timeout);
}

/**
* @brief  Read inbound data from the underlying socket stream.
*/
size_t scan::TcpClient::recv(char (&t_buffer)[BUFFER_SIZE], error_code &t_ecode)
{
    return recv(t_buffer, t_ecode, m_recv_timeout);
}

/**
* @brief  Read inbound data from the underlying socket stream.
*/
size_t scan::TcpClient::recv(char (&t_buffer)[BUFFER_SIZE],
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

        bytes_read = stream().read_some(mutable_buffer, t_ecode);
        m_ecode = t_ecode;
    }
    return bytes_read;
}

/**
* @brief  Get a constant reference to the underlying TCP socket stream.
*/
const scan::TcpClient::stream_t &scan::TcpClient::stream() const noexcept
{
    return *m_streamp;
}

/**
* @brief  Get a reference to the underlying TCP socket stream.
*/
scan::TcpClient::stream_t &scan::TcpClient::stream() noexcept
{
    return *m_streamp;
}

/**
* @brief  Get the most recent socket error code.
*/
boost::system::error_code scan::TcpClient::last_error() const noexcept
{
    return m_ecode;
}

/**
* @brief  Write the given string payload to the underlying socket stream.
*/
boost::system::error_code scan::TcpClient::send(const string &t_payload)
{
    return send(t_payload, m_send_timeout);
}

/**
* @brief  Write the given string payload to the underlying socket stream.
*/
boost::system::error_code scan::TcpClient::send(const string &t_payload,
                                                const Timeout &t_timeout) {
    if (connected_check())
    {
        send_timeout(t_timeout);

        // Send the payload
        if (connected_check() && !t_payload.empty())
        {
            stream().write_some(asio::buffer(t_payload), m_ecode);
        }
    }
    return m_ecode;
}

/**
* @brief  Get a constant reference to the underlying TCP socket.
*/
const scan::TcpClient::socket_t &scan::TcpClient::socket() const noexcept
{
    return stream().socket();
}

/**
* @brief  Get a reference to the underlying TCP socket.
*/
scan::TcpClient::socket_t &scan::TcpClient::socket() noexcept
{
    return stream().socket();
}

/**
* @brief  Read all the inbound data available from the underlying TCP socket stream.
*/
std::string scan::TcpClient::recv()
{
    return recv(m_ecode, m_recv_timeout);
}

/**
* @brief  Read all the inbound data available from the underlying TCP socket stream.
*/
std::string scan::TcpClient::recv(error_code &t_ecode)
{
    return recv(t_ecode, m_recv_timeout);
}

/**
* @brief  Read all the inbound data available from the underlying TCP socket stream.
*/
std::string scan::TcpClient::recv(error_code &t_ecode, const Timeout &t_timeout)
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
* @brief  Get a constant reference to the underlying remote TCP endpoint.
*/
const scan::Endpoint &scan::TcpClient::remote_ep() const noexcept
{
    Endpoint ep{ m_remote_ep };

    if (m_connected)
    {
        ep = socket().remote_endpoint();
    }
    return m_remote_ep;
}

/**
* @brief  Get a constant reference to the underlying network app service information.
*/
const scan::SvcInfo &scan::TcpClient::svcinfo() const noexcept
{
    return m_svc_info;
}

/**
* @brief  Get a reference to the underlying network app service information.
*/
scan::SvcInfo &scan::TcpClient::svcinfo() noexcept
{
    return m_svc_info;
}

/**
* @brief  Send the given HTTP request and return the server's response.
*/
scan::Response<> scan::TcpClient::request(const Request<> &t_request)
{
    if (!t_request.valid())
    {
        throw ArgEx{ "t_request", "Invalid HTTP request" };
    }
    Response response;

    // Perform HTTP communications
    if (connected_check())
    {
        http::write(stream(), t_request.message(), m_ecode);

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
* @brief  Send an HTTP request and return the server's response.
*/
scan::Response<> scan::TcpClient::request(const string &t_host, const string &t_uri)
{
    return request(verb_t::get, t_host, t_uri);
}

/**
* @brief  Send an HTTP request and return the server's response.
*/
scan::Response<> scan::TcpClient::request(const verb_t &t_method,
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
* @brief  Display error information and update the most recent error code.
*/
void scan::TcpClient::error(const error_code &t_ecode)
{
    const HostState state{ host_state(m_ecode = t_ecode) };

    if (m_verbose)
    {
        net::error(m_remote_ep, m_ecode);
    }

    if (m_trc_ap.load() == nullptr)
    {
        throw RuntimeEx{ "TcpClient::error", "Text resource pointer is null" };
    }
    net::update_svc(*m_trc_ap.load(), m_svc_info, state);
}

/**
* @brief  Callback handler for asynchronous connect operations.
*/
void scan::TcpClient::on_connect(const error_code &t_ecode, Endpoint t_ep)
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

    if (success_check())
    {
        if (m_verbose)
        {
            StdUtil::printf("Connection established: %/tcp", t_ep.port);
        }
        m_connected = true;
    }
}

/**
* @brief  Returns true if connected, otherwise false (and displays error).
*/
bool scan::TcpClient::connected_check()
{
    const bool connected{ is_connected() };

    // Display error info
    if (!connected)
    {
        error(error::not_connected);
    }
    return connected;
}

/**
* @brief  Returns true if no error occurred, otherwise false (and displays error).
*/
bool scan::TcpClient::success_check(const bool &t_eof_valid)
{
    return success_check(m_ecode, t_eof_valid);
}

/**
* @brief  Returns true if no error occurred, otherwise false (and displays error).
*/
bool scan::TcpClient::success_check(const error_code &t_ecode,
                                    const bool &t_eof_valid) {

    const bool success{ valid(m_ecode = t_ecode, t_eof_valid) };

    if (!success && host_state() != HostState::open)
    {
        error(t_ecode);
    }
    return success;
}

/**
* @brief  Determine whether the given error indicates a successful operation.
*/
bool scan::TcpClient::valid(const error_code &t_ecode,
                            const bool &t_eof_valid) noexcept {

    bool no_error{ net::no_error(t_ecode) };

    if (t_eof_valid)
    {
        no_error = no_error || t_ecode == error::eof;
    }
    return no_error;
}
