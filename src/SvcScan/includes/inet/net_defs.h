/*
*  net_defs.h
*  ----------
*  Header file for socket and networking type aliases and constants
*/
#pragma once

#ifndef NET_DEFS_H
#define NET_DEFS_H

#include <sdkddkver.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include "../utils/type_defs.h"

namespace scan
{
    namespace beast     = boost::beast;
    namespace ip        = asio::ip;
    namespace http      = beast::http;
    namespace ssl       = asio::ssl;
    namespace ssl_error = ssl::error;

    using beast_error  = beast::error;
    using header_map   = map<string, string>;
    using header_t     = header_map::value_type;
    using port_t       = word_t;
    using resolver_t   = ip::tcp::resolver;
    using results_t    = resolver_t::results_type;
    using socket_t     = ip::tcp::socket;
    using ssl_stream_t = beast::ssl_stream<beast::tcp_stream>;
    using status_t     = http::status;
    using stream_t     = beast::tcp_stream;
    using string_body  = http::string_body;
    using tcp          = ip::tcp;
    using verb_t       = http::verb;

    template<int SockOpt>
    using sock_opt = asio::detail::socket_option::integer<SOL_SOCKET, SockOpt>;

    constexpr port_t PORT_MAX  = WORD_MAX;           // Maximum port number
    constexpr port_t PORT_MIN  = 1U;                 // Minimum port number
    constexpr port_t PORT_NULL = WORD_NULL;          // Null port number

    constexpr uint_t CONN_TIMEOUT = 3500U;           // Default connect timeout
    constexpr uint_t RECV_TIMEOUT = 1000U;           // Default receive timeout
    constexpr uint_t SEND_TIMEOUT = 500U;            // Default send timeout

    constexpr int SOCKET_READY = 1;                  // WSA socket ready code

    constexpr size_t BUFFER_SIZE = 1024;             // Receive buffer size

    constexpr cstr_t<2> URI_ROOT   = { "/" };        // HTTP URI root path
    constexpr cstr_t<2> WILDCARD   = { "*" };        // MIME wild card
    constexpr cstr_t<4> PROTO      = { "tcp" };      // Default transport protocol
    constexpr cstr_t<5> PREFIX     = { "HTTP" };     // HTTP message version prefix
    constexpr cstr_t<6> CHARSET    = { "UTF-8" };    // MIME character set
    constexpr cstr_t<6> CONNECTION = { "close" };    // Default 'Connection' header
    constexpr cstr_t<8> IPV4_ANY   = { "0.0.0.0" };  // Unspecified IPv4 address
}

#endif // !NET_DEFS_H
