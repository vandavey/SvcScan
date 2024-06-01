/*
* @file
*     net_expr.h
* @brief
*     Header file for network and socket constant expressions.
*/
#pragma once

#ifndef NET_EXPR_H
#define NET_EXPR_H

#include "../utils/expr.h"
#include "net_defs.h"

namespace scan
{
    /// @brief  Maximum valid port number.
    constexpr port_t PORT_MAX = WORD_MAX;

    /// @brief  Minimum valid port number.
    constexpr port_t PORT_MIN = 1U;

    /// @brief  Null port number.
    constexpr port_t PORT_NULL = WORD_NULL;

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
    constexpr cstr_t CHARSET = "utf-8";

    /// @brief  'Connection' header.
    constexpr cstr_t CONNECTION = "close";

    /// @brief  Unspecified IPv4 address.
    constexpr cstr_t IPV4_ANY = "0.0.0.0";

    /// @brief  HTTP version prefix.
    constexpr cstr_t PREFIX = "HTTP";

    /// @brief  Transport protocol name.
    constexpr cstr_t PROTO = "tcp";

    /// @brief  HTTP URI root path.
    constexpr cstr_t URI_ROOT = "/";

    /// @brief  'User-Agent' header.
    constexpr cstr_t USER_AGENT = "SvcScan/1.0";

    /// @brief  MIME wild card.
    constexpr cstr_t WILDCARD = "*";
}

#endif // !NET_EXPR_H
