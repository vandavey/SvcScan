/*
* @file
*     net_aliases.h
* @brief
*     Header file for network and socket type aliases.
*/
#pragma once

#ifndef SCAN_NET_ALIASES_H
#define SCAN_NET_ALIASES_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <cstdint>
#include <map>
#include <winsock2.h>
#include <boost/asio/detail/socket_option.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include "../utils/aliases.h"

namespace scan
{
    namespace beast = boost::beast;
    namespace ip    = asio::ip;
    namespace http  = beast::http;
    namespace ssl   = asio::ssl;

    using flat_buffer  = beast::flat_buffer;
    using header_map   = map<string, string>;
    using header_t     = header_map::value_type;
    using port_t       = uint16_t;
    using resolver_t   = ip::tcp::resolver;
    using results_t    = resolver_t::results_type;
    using socket_t     = ip::tcp::socket;
    using ssl_stream_t = beast::ssl_stream<beast::tcp_stream>;
    using status_t     = http::status;
    using stream_t     = beast::tcp_stream;
    using string_body  = http::string_body;
    using verb_t       = http::verb;

    template<int SockOpt>
    using socket_option = asio::detail::socket_option::integer<SOL_SOCKET, SockOpt>;
}

#endif // !SCAN_NET_ALIASES_H
