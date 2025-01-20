/*
* @file
*     net.h
* @brief
*     Header file for network and socket utilities.
*/
#pragma once

#ifndef SCAN_NET_H
#define SCAN_NET_H

#include <algorithm>
#include <concepts>
#include <string>
#include <boost/asio/error.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/error.hpp>
#include <openssl/types.h>
#include "../concepts/concepts.h"
#include "../ranges/algo.h"
#include "../resources/text_rc.h"
#include "../utils/aliases.h"
#include "../utils/const_defs.h"
#include "net_aliases.h"
#include "net_const_defs.h"
#include "services/svc_info.h"
#include "sockets/endpoint.h"
#include "sockets/host_state.h"

/**
* @brief
*     Network and socket utilities.
*/
namespace scan::net
{
    /**
    * @brief
    *     Determine whether the given socket error code
    *     is an end of file or end of stream error.
    */
    constexpr bool eof_error(const error_code& t_ecode) noexcept
    {
        return algo::any_equal(t_ecode,
                               asio::error::eof,
                               http::error::end_of_stream,
                               ssl::error::stream_truncated);
    }

    /**
    * @brief
    *     Determine whether the given socket error code is an error.
    */
    constexpr bool is_error(const error_code& t_ecode,
                            bool t_allow_eof = false,
                            bool t_allow_partial_msg = false)
        noexcept
    {
        bool invalid{t_ecode.value() != RCODE_NO_ERROR};

        if (t_allow_eof && invalid)
        {
            invalid = !eof_error(t_ecode);
        }

        if (t_allow_partial_msg && invalid)
        {
            invalid = t_ecode != http::error::partial_message;
        }
        return invalid;
    }

    /**
    * @brief
    *     Determine whether the given socket error code is not an error.
    */
    constexpr bool no_error(const error_code& t_ecode,
                            bool t_allow_eof = false,
                            bool t_allow_partial_msg = false)
        noexcept
    {
        return !is_error(t_ecode, t_allow_eof, t_allow_partial_msg);
    }

    /**
    * @brief
    *     Determine whether the given socket error code is a timeout error.
    */
    constexpr bool timeout_error(const error_code& t_ecode) noexcept
    {
        return algo::any_equal(t_ecode, asio::error::timed_out, beast::error::timeout);
    }

    /**
    * @brief
    *     Determine whether the given network port number is valid.
    */
    template<std::integral T>
    constexpr bool valid_port(T t_port, bool t_ign_zero = false)
    {
        const T minimum_port{t_ign_zero ? PORT_NULL : PORT_MIN};
        return t_port >= minimum_port && t_port <= PORT_MAX;
    }

    /**
    * @brief
    *     Determine whether the network port numbers in the given range are valid.
    */
    template<IntegralRange R>
    constexpr bool valid_port(const R& t_ports, bool t_ign_zero = false)
    {
        return ranges::all_of(t_ports, [t_ign_zero](range_value_t<R> l_port) -> bool
        {
            return valid_port(l_port, t_ign_zero);
        });
    }

    /**
    * @brief
    *     Create an error message that corresponds to the given socket error.
    */
    constexpr string error_msg(const Endpoint& t_ep, const error_code& t_ecode)
    {
        string msg;

        switch (t_ecode.value())
        {
            case asio::error::host_not_found:
                msg = algo::fstr("Unable to resolve hostname: '%'", t_ep.addr);
                break;
            case asio::error::connection_refused:
                msg = algo::fstr("Connection refused: %/%", t_ep.port, PROTO);
                break;
            case asio::error::connection_reset:
                msg = algo::fstr("Connection was reset: %/%", t_ep.port, PROTO);
                break;
            case asio::error::would_block:
                msg = algo::fstr("Socket would block: %/%", t_ep.port, PROTO);
                break;
            case asio::error::timed_out:
            case asio::error::host_not_found_try_again:
            case static_cast<int>(beast::error::timeout):
                msg = algo::fstr("Connection timeout: %/%", t_ep.port, PROTO);
                break;
            default:
                msg = algo::fstr("%: '%'", t_ecode.value(), t_ecode);
                break;
        }
        return msg;
    }

    /**
    * @brief
    *     Create an error message that corresponds to the given TLS socket error.
    */
    constexpr string tls_error_msg(const Endpoint& t_ep, const error_code& t_ecode)
    {
        string msg;

        if (t_ecode == ssl::error::stream_truncated)
        {
            msg = algo::fstr("The TLS stream was closed: %/%", t_ep.port, PROTO);
        }
        else  // Unexpected result or unspecified error
        {
            msg = algo::fstr("An unknown TLS error occurred: %/%", t_ep.port, PROTO);
        }
        return msg;
    }

    /**
    * @brief
    *     Parse the string fields from the given CSV record line.
    */
    constexpr string_array<4> parse_fields(const string& t_csv_line)
    {
        return algo::split<4>(algo::erase(t_csv_line, "\""), ",");
    }

    void update_svc(const TextRc& t_csv_rc, SvcInfo& t_info, HostState t_state);

    bool valid_endpoint(const Endpoint& t_ep);
    bool valid_ipv4(const string& t_addr);
    bool valid_ipv4_fmt(const string& t_addr);
    bool valid_port(const string& t_port, bool t_ign_zero = false);

    string error(const Endpoint& t_ep, const error_code& t_ecode);
    string ipv4_from_results(const results_t& t_results);
    string x509_issuer(const X509* t_certp);
    string x509_name(X509_NAME* t_namep);
    string x509_subject(const X509* t_certp);

    results_t resolve(io_context& t_ioc,
                      const Endpoint& t_ep,
                      error_code& t_ecode,
                      uint_t t_retries = 0U);
}

#endif // !SCAN_NET_H
