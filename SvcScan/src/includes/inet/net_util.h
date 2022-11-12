/*
*  net_util.h
*  ----------
*  Header file for network and socket utilities
*/
#pragma once

#ifndef NET_UTIL_H
#define NET_UTIL_H

#include "../inet/net_defs.h"
#include "../resources/text_rc.h"
#include "../utils/timer.h"
#include "sockets/svc_info.h"

namespace scan
{
    /**
    * @brief  Network and socket utilities.
    */
    class NetUtil final
    {
    private:  /* Type Aliases */
        using algo = Algorithm;

    public:  /* Constructors & Destructor */
        NetUtil() = delete;
        NetUtil(const NetUtil &) = delete;
        NetUtil(NetUtil &&) = delete;

        virtual ~NetUtil() = default;

    public:  /* Operators */
        NetUtil &operator=(const NetUtil &) = default;
        NetUtil &operator=(NetUtil &&) = default;

    public:  /* Methods */
        static bool no_error(const error_code &t_ecode) noexcept;
        static bool valid_endpoint(const Endpoint &t_ep);
        static bool valid_ipv4(const string &t_addr);
        static bool valid_ipv4_fmt(const string &t_addr);
        static bool valid_port(const int &t_port, const bool &t_ign_zero = false);
        static bool valid_port(const string &t_port, const bool &t_ign_zero = false);

        static bool valid_port(const vector<uint_t> &t_ports,
                               const bool &t_ign_zero = false);

        static string error(const Endpoint &t_ep, const error_code &t_ecode);
        static string ipv4_from_results(const results_t &t_results);

        static SvcInfo update_svc(const TextRc &t_csv_rc,
                                  SvcInfo &t_info,
                                  const HostState &t_state);

        static results_t resolve(io_context &t_ioc,
                                 const Endpoint &t_ep,
                                 error_code &t_ecode,
                                 const uint_t &t_retries = 0U);

    private:  /* Methods */
        static string error_msg(const Endpoint &t_ep, const error_code &t_ecode);
        static string tls_error_msg(const Endpoint &t_ep, const error_code &t_ecode);

        static string_array<4> parse_fields(const string &t_csv_line);
    };
}

#endif // !NET_UTIL_H
