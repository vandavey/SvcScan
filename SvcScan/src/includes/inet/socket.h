/*
*  socket.h
*  --------
*  Header file for IPv4 TCP socket wrapper class
*/
#pragma once

#ifndef SOCKET_H
#define SOCKET_H

#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <string>
#include <vector>
#include <winsock2.h>
#include "../container/list.h"
#include "../except/nullptrex.h"
#include "../properties/property.h"
#include "../utils/util.h"
#include "svcinfo.h"

namespace scan
{
    /// ***
    /// Windows IPv4 TCP network socket wrapper
    /// ***
    class Socket
    {
    private:  /* Types */
        using uchar  = unsigned char;
        using uint   = unsigned int;
        using ulong  = unsigned long;
        using ushort = unsigned short;

        using string    = std::string;
        using list_si   = List<SvcInfo>;
        using list_ui   = List<uint>;
        using vector_ui = std::vector<uint>;

    private:  /* Constants */
        static constexpr char IPV4_ANY[] = "0.0.0.0";     // Any IPv4 address
        static constexpr ushort SOCKV{ MAKEWORD(2, 2) };  // WSA version

        static constexpr int BUFFER_SIZE{ 1024 };         // Socket buffer size
        static constexpr int SHUT_RD{ SD_RECEIVE };       // Halt communication
        static constexpr int SOCKET_READY{ 1 };           // Socket ready

    public:  /* Fields */
        Property<string> addr;    // Target address
        Property<list_ui> ports;  // Target ports

    private:  /* Fields */
        static timeval m_timeout;  // Connection timeout

        SOCKET m_sock;             // Underlying socket
        string m_addr;             // 'addr' backing field

        list_ui m_ports;           // 'ports' backing field
        list_si m_services;        // Service info

    public:  /* Constructors & Destructor */
        Socket();
        Socket(const Socket &t_sock);
        Socket(const string &t_addr, const list_ui &t_ports);

        virtual ~Socket();

    public:  /* Operators */
        Socket &operator=(const Socket &t_sock) noexcept;

    public:  /* Methods */
        static void set_timeout(const uint &t_sec, const uint &t_ms);

        static bool valid_port(const int &t_port);
        static bool valid_port(const string &t_port);
        static bool valid_port(const vector_ui &t_ports);

        static int valid_ip(const string &t_addr);

        void connect();

    private:  /* Methods */
        void close(addrinfoW *t_aiptr);
        void error() const;
        void error(const int &t_err) const;
        void error(const string &t_arg) const;
        void error(const int &t_err, const string &arg) const;

        bool valid_sock(const SOCKET &t_sock) const noexcept;

        HostState connect(addrinfoW *t_aiptr,
                          char (&t_buffer)[BUFFER_SIZE],
                          const EndPoint &t_ep);

        HostState recv(char (&t_buffer)[BUFFER_SIZE]);

        int get_error() const;

        int select(fd_set *t_rfds_ptr,
                   fd_set *t_wfds_ptr,
                   const timeval &t_to) const;

        int set_blocking(const bool &t_do_block);

        addrinfoW *startup(SvcInfo &t_si, const uint &t_port);

        SvcInfo &update_svc(SvcInfo &t_si, const HostState &t_hs) const;
    };
}

#endif // !SOCKET_H
