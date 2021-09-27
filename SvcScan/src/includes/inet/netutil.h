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
#include <winsock2.h>
#include "../conio/stdutil.h"
#include "../containers/list.h"
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

        using stdu   = StdUtil;
        using string = std::string;

        using array_s   = std::array<string, 4>;
        using list_ui   = List<uint>;
        using vector_a  = std::vector<array_s>;
        using vector_s  = std::vector<string>;
        using vector_ui = std::vector<uint>;

    public:  /* Constants */
        static constexpr ushort SOCKV{ MAKEWORD(2, 2) };  // WSA version

        static constexpr int MAX_PORT{ 65535 };           // Maximum network port
        static constexpr int SOCKET_READY{ 1 };           // Socket ready

        static constexpr char IPV4_ANY[] = "0.0.0.0";     // Any IPv4 address

    private:  /* Fields */
        static bool m_rc_loaded;       // Underlying resource loaded

        static uint m_wsa_call_count;  // WSAStartup call count

        static vector_a m_svcvect;     // Vector file buffer

    public:  /* Destructor */
        virtual ~NetUtil() = default;

    private:  /* Constructors (deleted) */
        NetUtil() = delete;
        NetUtil(const NetUtil &) = delete;

    public:  /* Methods */
        static void error(const string &t_addr);
        static void error(const EndPoint &t_ep, const int &t_err = NULL);
        static void free_info();
        static void load_info();

        static bool valid_port(const int &t_port);
        static bool valid_port(const string &t_port);
        static bool valid_port(const vector_ui &t_ports);
        static bool valid_sock(const SOCKET &t_sock) noexcept;

        static int get_error();
        static int set_blocking(SOCKET &t_sock, const bool &t_do_block);
        static int valid_ip(const string &t_addr);
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
        template<class T, size_t N>
        static array_s copy_n(const vector_s &t_vect);
    };
}

/// ***
/// Copy the specified number of vector elements into an array
/// ***
template<class T, size_t N>
inline scan::NetUtil::array_s scan::NetUtil::copy_n(const vector_s &t_vect)
{
    static_assert(N > 0, "Template parameter must be greater than zero");

    array_s buffer;

    // Copy elements into array
    for (size_t i{ 0 }; i < N; i++)
    {
        if (i == t_vect.size())
        {
            break;
        }
        buffer[i] = t_vect[i];
    }
    return buffer;
}

#endif // !NET_UTIL_H
