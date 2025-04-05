/*
* @file
*     net_const_defs.h
* @brief
*     Header file for network and socket constant expression fields.
*/
#pragma once

#ifndef SCAN_NET_CONST_DEFS_H
#define SCAN_NET_CONST_DEFS_H

#include "../utils/aliases.h"
#include "../utils/const_defs.h"
#include "../utils/literals.h"
#include "net_aliases.h"

namespace scan
{
    /// @brief  Maximum valid port number.
    constexpr port_t PORT_MAX = UINT16_T_MAX;

    /// @brief  Minimum valid port number.
    constexpr port_t PORT_MIN = 1_u16;

    /// @brief  Null port number.
    constexpr port_t PORT_NULL = UINT16_T_NULL;

    /// @brief  WSA socket ready code.
    constexpr int SOCKET_READY = 1;

    /// @brief  Receive buffer size.
    constexpr size_t BUFFER_SIZE = 1024_sz;

    /// @brief  Default connect timeout.
    constexpr milliseconds CONNECT_TIMEOUT = 3500_ms;

    /// @brief  Default receive timeout.
    constexpr milliseconds RECV_TIMEOUT = 1000_ms;

    /// @brief  Default send timeout.
    constexpr milliseconds SEND_TIMEOUT = 500_ms;

    /// @brief  MIME character set.
    constexpr c_string_t CHARSET_UTF8 = "utf-8";

    /// @brief  Close the connection.
    constexpr c_string_t CLOSE = "close";

    /// @brief  HTTP 'Accept' header.
    constexpr c_string_t HTTP_ACCEPT = "Accept";

    /// @brief  HTTP 'Connection' header.
    constexpr c_string_t HTTP_CONNECTION = "Connection";

    /// @brief  HTTP 'Content-Length' header.
    constexpr c_string_t HTTP_CONTENT_LENGTH = "Content-Length";

    /// @brief  HTTP 'Content-Type' header.
    constexpr c_string_t HTTP_CONTENT_TYPE = "Content-Type";

    /// @brief  HTTP 'Host' header.
    constexpr c_string_t HTTP_HOST = "Host";

    /// @brief  HTTP 'Server' header.
    constexpr c_string_t HTTP_SERVER = "Server";

    /// @brief  HTTP 'User-Agent' header.
    constexpr c_string_t HTTP_USER_AGENT = "User-Agent";

    /// @brief  Unspecified IPv4 address.
    constexpr c_string_t IPV4_ANY = "0.0.0.0";

    /// @brief  MIME subtype of 'application/json'.
    constexpr c_string_t MIME_SUBTYPE_JSON = "json";

    /// @brief  MIME subtype of 'text/plain'.
    constexpr c_string_t MIME_SUBTYPE_PLAIN = "plain";

    /// @brief  MIME subtype of 'application/xml'.
    constexpr c_string_t MIME_SUBTYPE_XML = "xml";

    /// @brief  MIME type 'application'.
    constexpr c_string_t MIME_TYPE_APPLICATION = "application";

    /// @brief  MIME type 'text'.
    constexpr c_string_t MIME_TYPE_TEXT = "text";

    /// @brief  MIME type wildcard.
    constexpr c_string_t MIME_TYPE_WILDCARD = "*";

    /// @brief  HTTP version prefix.
    constexpr c_string_t PREFIX = "HTTP";

    /// @brief  Transport protocol name.
    constexpr c_string_t PROTO = "tcp";

    /// @brief  Closed host state status name.
    constexpr c_string_t STATE_CLOSED = "closed";

    /// @brief  Open host state status name.
    constexpr c_string_t STATE_OPEN = "open";

    /// @brief  Unknown host state status name.
    constexpr c_string_t STATE_UNKNOWN = "unknown";

    /// @brief  Unknown service name.
    constexpr c_string_t SVC_UNKNOWN = "unknown";

    /// @brief  HTTP URI regular expression pattern.
    constexpr c_string_t URI_RGX = R"(^([!#$&-;=?-\[\]_a-z~]|%[0-9a-fA-F]{2})+$)";

    /// @brief  HTTP URI root path.
    constexpr c_string_t URI_ROOT = "/";

    /// @brief  Application 'User-Agent' value.
    constexpr c_string_t USER_AGENT = "SvcScan/1.0";
}

#endif // !SCAN_NET_CONST_DEFS_H
