/*
* @file
*     tcp_client.cpp
* @brief
*     Source file for an IPv4 TCP socket client.
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <memory>
#include <string>
#include <utility>
#include <winsock2.h>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/error.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/bind/bind.hpp>
#include "includes/console/util.h"
#include "includes/errors/arg_ex.h"
#include "includes/errors/error_const_defs.h"
#include "includes/errors/runtime_ex.h"
#include "includes/inet/sockets/tcp_client.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Initialize the object.
*/
scan::TcpClient::TcpClient(TcpClient&& t_client) noexcept : m_io_ctx{t_client.m_io_ctx}
{
    *this = std::move(t_client);
}

/**
* @brief
*     Initialize the object.
*/
scan::TcpClient::TcpClient(io_context& t_io_ctx,
                           shared_ptr<Args> t_argsp,
                           shared_ptr<TextRc> t_trcp)
    : m_io_ctx{t_io_ctx}
{
    m_connected = false;
    m_verbose = false;

    m_trc_ap = t_trcp;
    m_streamp = std::make_unique<stream_t>(m_io_ctx);

    parse_argsp(t_argsp);
}

/**
* @brief
*     Destroy the object.
*/
scan::TcpClient::~TcpClient()
{
    if (is_open())
    {
        error_code ecode;
        socket().close(ecode);
    }
}

/**
* @brief
*     Move assignment operator overload.
*/
scan::TcpClient& scan::TcpClient::operator=(TcpClient&& t_client) noexcept
{
    if (this != &t_client)
    {
        m_args_ap = std::move(t_client.m_args_ap.load());
        m_connected = t_client.m_connected;
        m_ecode = std::move(t_client.m_ecode);
        m_remote_ep = std::move(t_client.m_remote_ep);
        m_streamp = std::move(t_client.m_streamp);
        m_svc_info = std::move(t_client.m_svc_info);
        m_timeout = std::move(t_client.m_timeout);
        m_trc_ap = std::move(t_client.m_trc_ap.load());
        m_verbose = t_client.m_verbose;
    }
    return *this;
}

/**
* @brief
*     Close the underlying TCP socket.
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
* @brief
*     Establish a network connection to the given TCP endpoint.
*/
void scan::TcpClient::connect(const Endpoint& t_ep)
{
    if (!net::valid_endpoint(m_remote_ep = t_ep))
    {
        throw ArgEx{INVALID_ENDPOINT_MSG, "t_ep"};
    }

    m_svc_info.addr = t_ep.addr;
    m_svc_info.port(t_ep.port);

    // Perform DNS name resolution
    const results_t results{net::resolve(m_io_ctx, m_remote_ep, m_ecode)};

    // Establish the connection
    if (success_check())
    {
        async_connect(results);
        async_await();
    }
}

/**
* @brief
*     Establish a network connection to underlying target on the given port.
*/
void scan::TcpClient::connect(port_t t_port)
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
            throw RuntimeEx{INVALID_TARGET_MSG, "TcpClient::connect"};
        }
        m_remote_ep = {m_args_ap.load()->target.addr(), t_port};
    }
    connect(m_remote_ep);
}

/**
* @brief
*     Disconnect from the remote host and close the underlying TCP socket.
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
* @brief
*     Shutdown further communications on the underlying TCP socket.
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
* @brief
*     Determine whether the underlying TCP socket is open.
*/
bool scan::TcpClient::is_open() const noexcept
{
    return socket().is_open();
}

/**
* @brief
*     Read inbound data from the underlying socket stream.
*/
size_t scan::TcpClient::recv(buffer_t& t_buffer)
{
    string data;
    size_t bytes_read{0_sz};

    if (connected_check())
    {
        recv_timeout(RECV_TIMEOUT);

        const mutable_buffer mut_buffer{&t_buffer[0], sizeof t_buffer};
        bytes_read = stream().read_some(mut_buffer, m_ecode);
    }
    return bytes_read;
}

/**
* @brief
*     Write the given string payload to the underlying socket stream.
*/
size_t scan::TcpClient::send(const string& t_payload)
{
    size_t bytes_sent{0_sz};

    if (connected_check())
    {
        send_timeout(SEND_TIMEOUT);

        if (connected_check() && !t_payload.empty())
        {
            bytes_sent = stream().write_some(asio::buffer(t_payload), m_ecode);
        }
    }
    return bytes_sent;
}

/**
* @brief
*     Send the given HTTP request and return the server's response.
*/
scan::Response<> scan::TcpClient::request(const Request<>& t_request)
{
    if (!t_request.valid())
    {
        throw ArgEx{INVALID_REQUEST_MSG, "t_request"};
    }
    Response response;

    // Perform HTTP communications
    if (connected_check())
    {
        http::write(stream(), t_request.message(), m_ecode);

        if (success_check())
        {
            http::response_parser<string_body> parser;
            beast::flat_buffer& buffer{response.buffer};

            size_t bytes_read{http::read_header(stream(), buffer, parser, m_ecode)};

            if (m_ecode != http::error::bad_version && success_check(true, true))
            {
                do  // Read until end reached or message fully parsed
                {
                    bytes_read = http::read(stream(), buffer, parser, m_ecode);
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
*     Send an HTTP request and return the server's response.
*/
scan::Response<> scan::TcpClient::request(const string& t_host, const string& t_uri)
{
    return request(verb_t::get, t_host, t_uri);
}

/**
* @brief
*     Send an HTTP request and return the server's response.
*/
scan::Response<> scan::TcpClient::request(verb_t t_method,
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
*     Await the completion of the most recent asynchronous operation.
*/
void scan::TcpClient::async_await()
{
    m_io_ctx.run();
    m_io_ctx.restart();
}

/**
* @brief
*     Asynchronously establish a network connection on the underlying
*     TCP socket. Does not wait for completion and returns immediately.
*/
void scan::TcpClient::async_connect(const results_t& t_results)
{
    auto connect_callback = boost::bind(&TcpClient::on_connect,
                                        this,
                                        asio::placeholders::error,
                                        asio::placeholders::endpoint);

    stream().expires_after(static_cast<milliseconds>(m_timeout));
    stream().async_connect(t_results, std::move(connect_callback));
}

/**
* @brief
*     Display error information and update the most recent error code.
*/
void scan::TcpClient::error(const error_code& t_ecode)
{
    m_ecode = t_ecode;
    const HostState state{host_state()};

    if (m_verbose)
    {
        net::error(m_remote_ep, m_ecode);
    }

    if (m_trc_ap.load() == nullptr)
    {
        throw RuntimeEx{NULL_PTR_DEREF_MSG, "TcpClient::error"};
    }
    net::update_svc(*m_trc_ap.load(), m_svc_info, state);
}

/**
* @brief
*     Callback handler for asynchronous connect operations.
*/
void scan::TcpClient::on_connect(const error_code& t_ecode, Endpoint t_ep)
{
    m_ecode = t_ecode;

    // Ensure socket error and host state accuracy
    if (m_ecode == asio::error::host_not_found)
    {
        m_ecode = asio::error::connection_refused;
        m_svc_info.state(HostState::closed);
    }
    else if (net::is_error(m_ecode))
    {
        m_svc_info.state(HostState::unknown);
    }

    if (success_check())
    {
        if (m_verbose)
        {
            util::printf("Connection established: %/%", t_ep.port, PROTO);
        }
        m_connected = true;
    }
}

/**
* @brief
*     Parse information from the given command-line arguments smart pointer.
*/
void scan::TcpClient::parse_argsp(shared_ptr<Args> t_argsp)
{
    m_args_ap = t_argsp;
    m_svc_info.addr = t_argsp->target;
    m_timeout = t_argsp->timeout;
    m_verbose = t_argsp->verbose;
}

/**
* @brief
*     Set the timeout for synchronous socket receive operations.
*/
void scan::TcpClient::recv_timeout(const Timeout& t_timeout)
{
    set_timeout<SO_RCVTIMEO>(t_timeout);
}

/**
* @brief
*     Set the timeout for synchronous socket send operations.
*/
void scan::TcpClient::send_timeout(const Timeout& t_timeout)
{
    set_timeout<SO_SNDTIMEO>(t_timeout);
}

/**
* @brief
*     Returns true if connected, otherwise false (and displays error).
*/
bool scan::TcpClient::connected_check()
{
    const bool connected{is_connected()};

    if (!connected)
    {
        error(asio::error::not_connected);
    }
    return connected;
}

/**
* @brief
*     Returns true if no error occurred, otherwise false (and displays error).
*/
bool scan::TcpClient::success_check(bool t_allow_eof, bool t_allow_partial_msg)
{
    return success_check(m_ecode, t_allow_eof, t_allow_partial_msg);
}

/**
* @brief
*     Returns true if no error occurred, otherwise false (and displays error).
*/
bool scan::TcpClient::success_check(const error_code& t_ecode,
                                    bool t_allow_eof,
                                    bool t_allow_partial_msg)
{
    m_ecode = t_ecode;
    const bool success{valid(m_ecode, t_allow_eof, t_allow_partial_msg)};

    if (!success && host_state() != HostState::open)
    {
        error(m_ecode);
    }
    return success;
}

/**
* @brief
*     Get a constant reference to the underlying TCP socket stream.
*/
const scan::stream_t& scan::TcpClient::stream() const noexcept
{
    return *m_streamp;
}

/**
* @brief
*     Get a reference to the underlying TCP socket stream.
*/
scan::stream_t& scan::TcpClient::stream() noexcept
{
    return *m_streamp;
}

/**
* @brief
*     Get a constant reference to the underlying remote TCP endpoint.
*/
const scan::Endpoint& scan::TcpClient::remote_ep() const noexcept
{
    Endpoint ep{m_remote_ep};

    if (m_connected)
    {
        ep = socket().remote_endpoint();
    }
    return m_remote_ep;
}

/**
* @brief
*     Get a constant reference to the underlying TCP socket.
*/
const scan::socket_t& scan::TcpClient::socket() const noexcept
{
    return stream().socket();
}

/**
* @brief
*     Get a reference to the underlying TCP socket.
*/
scan::socket_t& scan::TcpClient::socket() noexcept
{
    return stream().socket();
}
