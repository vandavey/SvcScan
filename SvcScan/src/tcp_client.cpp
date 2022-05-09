/*
*  tcp_client.cpp
*  --------------
*  Source file for an IPv4 TCP socket client
*/
#include <sdkddkver.h>
#include <boost/asio/placeholders.hpp>
#include <boost/bind/bind.hpp>
#include "includes/inet/sockets/tcp_client.h"

/// ***
/// Initialize the object
/// ***
scan::TcpClient::TcpClient(TcpClient &&t_client) noexcept : m_ioc(t_client.m_ioc)
{
    m_connected = t_client.m_connected;
    m_conn_timeout = t_client.m_conn_timeout;
    m_csv_rc = std::move(t_client.m_csv_rc);
    m_ecode = t_client.m_ecode;
    m_remote_ep = t_client.m_remote_ep;
    m_streamp = std::move(t_client.m_streamp);
    m_svc_info = t_client.m_svc_info;
    m_verbose = t_client.m_verbose;
}

/// ***
/// Initialize the object
/// ***
scan::TcpClient::TcpClient(io_context &t_ioc, const Args &t_args) : m_ioc(t_ioc)
{
    m_connected = false;
    m_csv_rc = CSV_DATA;
    m_streamp = std::make_unique<stream_t>(m_ioc);
    m_verbose = false;

    parse_args(t_args);
}

/// ***
/// Destroy the object
/// ***
scan::TcpClient::~TcpClient()
{
    if (is_open())
    {
        error_code discard_ecode;
        socket().close(discard_ecode);
    }
}

/// ***
/// Await the completion of the most recent asynchronous operation
/// ***
void scan::TcpClient::await_operation(const bool &t_restart)
{
    m_ioc.run();

    if (t_restart)
    {
        m_ioc.restart();
    }
}

/// ***
/// Close the underlying socket
/// ***
void scan::TcpClient::close()
{
    if (is_open())
    {
        error_code ecode;
        socket().close(ecode);
        success_check(ecode);
    }
    m_svc_info.reset(m_remote_ep.addr);
}

/// ***
/// Establish a network connection using the underlying TCP socket
/// ***
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
    if (success_check(m_ecode))
    {
        connect(results, m_conn_timeout);
    }
}

/// ***
/// Establish a network connection using the underlying TCP socket
/// ***
void scan::TcpClient::connect(const uint &t_port)
{
    // Invalid network port
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number" };
    }

    // Unknown remote host address
    if (m_remote_ep.addr.empty() || (m_remote_ep.addr == Endpoint::IPV4_ANY))
    {
        throw RuntimeEx{ "TcpClient::connect", "Invalid underlying remote host" };
    }
    connect(m_remote_ep);
}

/// ***
/// Set the underlying socket connection timeout value
/// ***
void scan::TcpClient::connect_timeout(const Timeout &t_timeout)
{
    m_conn_timeout = t_timeout;
}

/// ***
/// Disconnect and close the underlying TCP socket
/// ***
void scan::TcpClient::disconnect()
{
    if (connected_check() && is_open())
    {
        shutdown();
        close();

        m_connected = false;
    }
}

/// ***
/// Parse information from the given command-line arguments
/// ***
void scan::TcpClient::parse_args(const Args &t_args) noexcept
{
    m_svc_info.addr = t_args.target;
    m_conn_timeout = t_args.timeout;
    m_verbose = t_args.verbose;
}

/// ***
/// Set the timeout period for synchronous receive operations
/// ***
void scan::TcpClient::recv_timeout(const Timeout &t_timeout)
{
    set_timeout<SO_RCVTIMEO>(t_timeout);
}

/// ***
/// Set the timeout period for synchronous send operations
/// ***
void scan::TcpClient::send_timeout(const Timeout &t_timeout)
{
    set_timeout<SO_SNDTIMEO>(t_timeout);
}

/// ***
/// Shutdown further communications on the underlying TCP socket
/// ***
void scan::TcpClient::shutdown()
{
    if (connected_check() && is_open())
    {
        socket().shutdown(socket_t::shutdown_both, m_ecode);
        success_check();
    }
}

/// ***
/// Determine whether the underlying TCP socket is connected
/// ***
bool scan::TcpClient::is_connected() const noexcept
{
    return m_connected;
}

/// ***
/// Determine whether the underlying TCP socket is open
/// ***
bool scan::TcpClient::is_open() const noexcept
{
    return socket().is_open();
}

/// ***
/// Get the most recent socket error code
/// ***
boost::system::error_code scan::TcpClient::last_error() const noexcept
{
    return m_ecode;
}

/// ***
/// Send the given string payload to the connected remote endpoint
/// ***
scan::TcpClient::error_code scan::TcpClient::send(const string &t_payload,
                                                  const Timeout &t_timeout) {
    if (connected_check())
    {
        send_timeout(t_timeout);

        // Send the payload
        if (connected_check() && !t_payload.empty())
        {
            m_streamp->write_some(asio::buffer(t_payload), m_ecode);
        }
    }
    return m_ecode;
}

/// ***
/// Retrieve a constant reference to the underlying TCP socket
/// ***
const scan::TcpClient::socket_t &scan::TcpClient::socket() const noexcept
{
    return m_streamp->socket();
}

/// ***
/// Retrieve a reference to the underlying TCP socket
/// ***
scan::TcpClient::socket_t &scan::TcpClient::socket() noexcept
{
    return m_streamp->socket();
}

/// ***
/// Read all inbound data currently available in the underlying socket stream
/// ***
std::string scan::TcpClient::recv(error_code &t_ecode, const Timeout &t_timeout)
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
    while (valid(t_ecode) && (bytes_read > 0));

    return data.str();
}

/// ***
/// Get a reference to the underlying application service information
/// ***
const scan::SvcInfo &scan::TcpClient::svcinfo() const noexcept
{
    return m_svc_info;
}

/// ***
/// Get a reference to the underlying application service information
/// ***
scan::SvcInfo &scan::TcpClient::svcinfo() noexcept
{
    return m_svc_info;
}

/// ***
/// Get a constant reference to the underlying embedded text resource
/// ***
const scan::TextRc &scan::TcpClient::textrc() const noexcept
{
    return m_csv_rc;
}

/// ***
/// Utility - Determine whether the given error indicates a successful operation
/// ***
bool scan::TcpClient::valid(const error_code &t_ecode,
                            const bool &t_eof_valid) noexcept {

    bool no_error{ net::no_error(t_ecode) };

    // Consider EOF errors valid
    if (t_eof_valid)
    {
        no_error = no_error || (t_ecode == error::eof);
    }
    return no_error;
}

/// ***
/// Display error information and update the most recent error code
/// ***
void scan::TcpClient::error(const error_code &t_ecode)
{
    const HostState state{ net::host_state(m_ecode = t_ecode, m_connected) };

    if (m_verbose)
    {
        net::error(m_remote_ep, m_ecode);
    }
    net::update_svc(m_csv_rc, m_svc_info, state);
}

/// ***
/// Callback handler method for asynchronous connect operations
/// ***
void scan::TcpClient::on_connect(const error_code &t_ecode, Endpoint t_ep)
{
    m_ecode = t_ecode;

    // Ensure accuracy of error/state
    if (m_ecode == error::host_not_found)
    {
        m_svc_info.state = HostState::closed;
        m_ecode = error::connection_refused;
    }
    else if (!net::no_error(m_ecode))
    {
        m_svc_info.state = HostState::unknown;
    }

    if (success_check(m_ecode))
    {
        if (m_verbose)
        {
            StdUtil::printf("Connection established: %/tcp", t_ep.port);
        }
        m_connected = true;
    }
}

/// ***
/// Returns true if currently connected, otherwise false (displays error info)
/// ***
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

/// ***
/// Returns true if no error occurred, otherwise false (displays error info)
/// ***
bool scan::TcpClient::success_check()
{
    return success_check(m_ecode);
}

/// ***
/// Returns true if no error occurred, otherwise false (displays error info)
/// ***
bool scan::TcpClient::success_check(const error_code &t_ecode)
{
    const bool success{ valid(m_ecode = t_ecode, true) };
    const HostState state{ net::host_state(m_ecode, m_connected) };

    // Display error info
    if (!success && (state != HostState::open))
    {
        error(t_ecode);
    }
    return success;
}

/// ***
/// Establish a network connection using the underlying TCP socket
/// ***
boost::system::error_code scan::TcpClient::connect(const results_t &t_results,
                                                   const Timeout &t_timeout) {

    m_streamp->expires_after(static_cast<chrono::milliseconds>(t_timeout));

    // Connect underlying socket asynchronously
    m_streamp->async_connect(t_results, boost::bind(&TcpClient::on_connect,
                                                    this,
                                                    asio::placeholders::error,
                                                    asio::placeholders::endpoint));
    await_operation();

    return m_ecode;
}
