/*
*  netutil.h
*  ---------
*  Header file for network and socket utilities
*/
#pragma once

#ifndef NET_UTIL_H
#define NET_UTIL_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <array>
#include <string>
#include <vector>
#include <ws2tcpip.h>
#include "../containers/generic/list.h"
#include "../io/stdutil.h"
#include "../rc/textrc.h"
#include "../utils/timer.h"
#include "sockets/endpoint.h"
#include "svcinfo.h"

namespace scan
{
    /// ***
    /// Network and socket utilities
    /// ***
    class NetUtil final
    {
    private:  /* Type Aliases */
        using uint   = unsigned int;
        using ulong  = unsigned long;
        using ushort = unsigned short;

        using stdu    = StdUtil;
        using sstream = std::stringstream;
        using string  = std::string;

        using array_s   = std::array<string, 4>;
        using list_ui   = List<uint>;
        using vector_a  = std::vector<array_s>;
        using vector_s  = std::vector<string>;
        using vector_ui = std::vector<uint>;

    public:  /* Constants */
        static constexpr ushort SOCKV{ MAKEWORD(2, 2) };  // WSA version

        static constexpr int MAX_PORT{ 65535 };           // Maximum network port
        static constexpr int MIN_PORT{ 1 };               // Minimum network port
        static constexpr int SOCKET_READY{ 1 };           // Socket ready

        static constexpr char IPV4_ANY[] = "0.0.0.0";     // Any IPv4 address

    private:  /* Fields */
        static uint m_wsa_call_count;  // WSAStartup() call count

        static TextRc m_csv_rc;        // CSV text resource

    public:  /* Destructor */
        virtual ~NetUtil() = default;

    private:  /* Constructors (deleted) */
        NetUtil() = delete;
        NetUtil(const NetUtil &) = delete;

    public:  /* Methods */
        static void error(const string &t_addr);
        static void error(const Endpoint &t_ep, const int &t_err = NULL);

        static bool valid_endpoint(const Endpoint &t_ep);
        static bool valid_ipv4(const string &t_addr);
        static bool valid_ipv4_fmt(const string &t_addr);
        static bool valid_port(const int &t_port, const bool &t_ign_zero = false);
        static bool valid_port(const string &t_port, const bool &t_ign_zero = false);

        static bool valid_port(const vector_ui &t_ports,
                               const bool &t_ign_zero = false);

        static bool valid_sock(const SOCKET &t_sock) noexcept;

        static int get_error();
        static int set_blocking(SOCKET &t_sock, const bool &t_do_block);
        static int wsa_cleanup();
        static int wsa_startup(const string &t_addr);

        static string scan_progress(const uint &t_next_port,
                                    const list_ui &t_ports,
                                    const size_t &t_start_pos);

        static string scan_summary(const string &t_target,
                                   const Timer &t_timer,
                                   const string &t_outpath = string());

        static SvcInfo update_svc(SvcInfo &t_si, const HostState &t_hs);

    private: /* Methods */
        static array_s parse_fields(const string &t_csv_line);
    };
}

#endif // !NET_UTIL_H
