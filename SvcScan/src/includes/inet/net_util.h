/*
*  net_util.h
*  ----------
*  Header file for network and socket utilities
*/
#pragma once

#ifndef NET_UTIL_H
#define NET_UTIL_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <boost/beast/core/error.hpp>
#include "../rc/text_rc.h"
#include "../utils/timer.h"
#include "sockets/svc_info.h"

namespace
{
    namespace error = boost::asio::error;
}

namespace scan
{
    /// ***
    /// Network and socket utilities
    /// ***
    class NetUtil final
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using array_s     = std::array<std::string, 4>;
        using beast_error = boost::beast::error;
        using error_code  = boost::system::error_code;
        using io_context  = boost::asio::io_context;
        using results_t   = boost::asio::ip::tcp::resolver::results_type;
        using string      = std::string;
        using tcp         = boost::asio::ip::tcp;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Constants */
        static constexpr int MAX_PORT{ 65535 };  // Maximum port number
        static constexpr int MIN_PORT{ 1 };      // Minimum port number
        static constexpr int SOCKET_READY{ 1 };  // WSA socket ready code

    public:  /* Constructors & Destructor */
        NetUtil() = delete;
        NetUtil(const NetUtil &) = delete;

        virtual ~NetUtil() = default;

    public:  /* Methods */
        static bool no_error(const error_code &t_ecode) noexcept;
        static bool valid_endpoint(const Endpoint &t_ep);

        static bool valid_ipv4(const string &t_addr);
        static bool valid_ipv4_fmt(const string &t_addr);
        static bool valid_port(const int &t_port, const bool &t_ign_zero = false);
        static bool valid_port(const string &t_port, const bool &t_ign_zero = false);

        static bool valid_port(const vector<uint> &t_ports,
                               const bool &t_ign_zero = false);

        static HostState host_state(const error_code &t_ecode,
                                    const bool &t_connected) noexcept;

        static string error(const Endpoint &t_ep, const error_code &t_ecode);
        static string ipv4_from_results(const results_t &t_results);

        static SvcInfo update_svc(const TextRc &t_csv_rc,
                                  SvcInfo &t_si,
                                  const HostState &t_hs);

        static results_t resolve(io_context &t_ioc,
                                 const Endpoint &t_ep,
                                 error_code &t_ecode,
                                 const uint &t_retries = 0U);

    private:  /* Methods */
        static array_s parse_fields(const string &t_csv_line);
    };
}

#endif // !NET_UTIL_H
