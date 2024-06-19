/*
* @file
*     net_util.h
* @brief
*     Header file for network and socket utilities.
*/
#pragma once

#ifndef SCAN_NET_UTIL_H
#define SCAN_NET_UTIL_H

#include <algorithm>
#include <concepts>
#include <openssl/types.h>
#include "../concepts/concepts.h"
#include "../resources/text_rc.h"
#include "../utils/alias.h"
#include "net_alias.h"
#include "net_const_defs.h"
#include "sockets/endpoint.h"
#include "sockets/host_state.h"
#include "sockets/svc_info.h"

namespace scan
{
    /**
    * @brief
    *     Network and socket utilities.
    */
    class NetUtil final
    {
    public:  /* Constructors & Destructor */
        NetUtil() = delete;
        NetUtil(const NetUtil &) = delete;
        NetUtil(NetUtil &&) = delete;

        virtual ~NetUtil() = default;

    public:  /* Operators */
        NetUtil &operator=(const NetUtil &) = default;
        NetUtil &operator=(NetUtil &&) = default;

    public:  /* Methods */
        static void update_svc(const TextRc &t_csv_rc,
                               SvcInfo &t_info,
                               const HostState &t_state);

        static bool no_error(const error_code &t_ecode) noexcept;
        static bool valid_endpoint(const Endpoint &t_ep);
        static bool valid_ipv4(const string &t_addr);
        static bool valid_ipv4_fmt(const string &t_addr);
        static bool valid_port(const string &t_port, const bool &t_ign_zero = false);

        template<std::integral T>
        static bool valid_port(const T &t_port, const bool &t_ign_zero = false);

        template<IntegralRange R>
        static bool valid_port(const R &t_ports, const bool &t_ign_zero = false);

        static string error(const Endpoint &t_ep, const error_code &t_ecode);
        static string ipv4_from_results(const results_t &t_results);
        static string x509_issuer(const X509 *t_certp);
        static string x509_subject(const X509 *t_certp);

        static results_t resolve(io_context &t_ioc,
                                 const Endpoint &t_ep,
                                 error_code &t_ecode,
                                 const uint_t &t_retries = 0U);

    private:  /* Methods */
        static string error_msg(const Endpoint &t_ep, const error_code &t_ecode);
        static string tls_error_msg(const Endpoint &t_ep, const error_code &t_ecode);
        static string x509_name(X509_NAME *t_namep);

        static string_array<4> parse_fields(const string &t_csv_line);
    };
}

/**
* @brief
*     Determine whether the given network port number is valid.
*/
template<std::integral T>
inline bool scan::NetUtil::valid_port(const T &t_port, const bool &t_ign_zero)
{
    const T minimum_port{ t_ign_zero ? PORT_NULL : PORT_MIN };
    return t_port >= minimum_port && t_port <= PORT_MAX;
}

/**
* @brief
*     Determine whether the network port numbers in the given range are valid.
*/
template<scan::IntegralRange R>
inline bool scan::NetUtil::valid_port(const R &t_ports, const bool &t_ign_zero)
{
    return ranges::all_of(t_ports, [&t_ign_zero](const range_value_t<R> &l_port)
    {
        return valid_port(l_port, t_ign_zero);
    });
}

#endif // !SCAN_NET_UTIL_H
