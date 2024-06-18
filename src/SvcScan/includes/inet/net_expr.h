/*
* @file
*     net_expr.h
* @brief
*     Header file for network and socket constant expression fields.
*/
#pragma once

#ifndef SCAN_NET_EXPR_H
#define SCAN_NET_EXPR_H

#include "../utils/alias.h"
#include "../utils/expr.h"
#include "net_alias.h"

namespace scan
{
    /// @brief  Maximum valid port number.
    constexpr port_t PORT_MAX = UINT16_T_MAX;

    /// @brief  Minimum valid port number.
    constexpr port_t PORT_MIN = 1U;

    /// @brief  Null port number.
    constexpr port_t PORT_NULL = UINT16_T_NULL;

    /// @brief  Default connect timeout.
    constexpr uint_t CONN_TIMEOUT = 3500U;

    /// @brief  Default receive timeout.
    constexpr uint_t RECV_TIMEOUT = 1000U;

    /// @brief  Default send timeout.
    constexpr uint_t SEND_TIMEOUT = 500U;

    /// @brief  WSA socket ready code.
    constexpr int SOCKET_READY = 1;

    /// @brief  Receive buffer size.
    constexpr size_t BUFFER_SIZE = 1024;

    /// @brief  MIME character set.
    constexpr cstr_t CHARSET_UTF8 = "utf-8";

    /// @brief  Close the connection.
    constexpr cstr_t CLOSE = "close";

    /// @brief  HTTP 'Accept' header.
    constexpr cstr_t HTTP_ACCEPT = "Accept";

    /// @brief  HTTP 'Connection' header.
    constexpr cstr_t HTTP_CONNECTION = "Connection";

    /// @brief  HTTP 'Content-Length' header.
    constexpr cstr_t HTTP_CONTENT_LENGTH = "Content-Length";

    /// @brief  HTTP 'Content-Type' header.
    constexpr cstr_t HTTP_CONTENT_TYPE = "Content-Type";

    /// @brief  HTTP 'Host' header.
    constexpr cstr_t HTTP_HOST = "Host";

    /// @brief  HTTP 'Server' header.
    constexpr cstr_t HTTP_SERVER = "Server";

    /// @brief  HTTP 'User-Agent' header.
    constexpr cstr_t HTTP_USER_AGENT = "User-Agent";

    /// @brief  Unspecified IPv4 address.
    constexpr cstr_t IPV4_ANY = "0.0.0.0";

    /// @brief  MIME subtype of 'application/json'.
    constexpr cstr_t MIME_SUBTYPE_JSON = "json";

    /// @brief  MIME subtype of 'text/plain'.
    constexpr cstr_t MIME_SUBTYPE_PLAIN = "plain";

    /// @brief  MIME subtype of 'application/xml'.
    constexpr cstr_t MIME_SUBTYPE_XML = "xml";

    /// @brief  MIME type 'application'.
    constexpr cstr_t MIME_TYPE_APPLICATION = "application";

    /// @brief  MIME type 'text'.
    constexpr cstr_t MIME_TYPE_TEXT = "text";

    /// @brief  MIME type wildcard.
    constexpr cstr_t MIME_TYPE_WILDCARD = "*";

    /// @brief  HTTP version prefix.
    constexpr cstr_t PREFIX = "HTTP";

    /// @brief  Transport protocol name.
    constexpr cstr_t PROTO = "tcp";

    /// @brief  HTTP URI regular expression pattern.
    constexpr cstr_t URI_RGX = R"(^([!#$&-;=?-\[\]_a-z~]|%[0-9a-fA-F]{2})+$)";

    /// @brief  HTTP URI root path.
    constexpr cstr_t URI_ROOT = "/";

    /// @brief  Application 'User-Agent' value.
    constexpr cstr_t USER_AGENT = "SvcScan/1.0";
}

#endif // !SCAN_NET_EXPR_H
