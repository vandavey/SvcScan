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

#include <string>
#include <vector>
#include <winsock2.h>
#include "../conio/stdutil.h"
#include "../utils/timer.h"
#include "endpoint.h"
#include "svcinfo.h"

namespace scan
{
    /// ***
    /// Network and socket utilities
    /// ***
    class NetUtil final
    {
    private:  /* Types */
        using uint   = unsigned int;
        using ulong  = unsigned long;
        using ushort = unsigned short;

        using stdu      = StdUtil;
        using string    = std::string;
        using vector_ui = std::vector<uint>;

    public:  /* Constants */
        static constexpr ushort SOCKV{ MAKEWORD(2, 2) };  // WSA version

        static constexpr int SOCKET_READY{ 1 };           // Socket ready

        static constexpr char IPV4_ANY[] = "0.0.0.0";     // Any IPv4 address

    private:  /* Fields */
        static uint m_wsa_call_count;  // WSAStartup call count

    public:  /* Destructor */
        virtual ~NetUtil();

    private:  /* Constructors (deleted) */
        NetUtil() = delete;
        NetUtil(const NetUtil &) = delete;

    public:  /* Methods */
        static void error(const string &t_addr);
        static void error(const EndPoint &t_ep, const int &t_err = NULL);

        static bool valid_port(const int &t_port);
        static bool valid_port(const string &t_port);
        static bool valid_port(const vector_ui &t_ports);
        static bool valid_sock(const SOCKET &t_sock) noexcept;

        static int get_error();
        static int set_blocking(SOCKET &t_sock,  const bool &t_do_block);
        static int valid_ip(const string &t_addr);
        static int wsa_cleanup();
        static int wsa_startup(const string &t_addr);

        static string scan_summary(const string &t_target,
                                   const Timer &t_timer,
                                   const string &t_outpath = string());

        static SvcInfo update_svc(SvcInfo &t_si, const HostState &t_hs);
    };
}

#endif // !NET_UTIL_H
