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
        using llong = long long;
        using uchar = unsigned char;
        using ulong = unsigned long;
        using ushort = unsigned short;

        using string = std::string;
        using list_s = List<string>;
        using list_si = List<SvcInfo>;
        using vector_s = std::vector<string>;

        using property_l = Property<list_s>;
        using property_s = Property<string>;

    private:  /* Constants */
        static constexpr ushort SOCKV{ (2 << 8) | 2 };  // WSA version
        static constexpr int BUFFER_SIZE{ 1024 };       // Socket buffer size
        static constexpr int SHUT_RD{ SD_RECEIVE };     // Halt communication

    public:  /* Fields */
        SOCKET m_sock;     // Underlying socket
        property_s addr;   // Target address
        property_l ports;  // Target ports

    protected:  /* Fields */
        string m_addr;       // 'addr' backing field
        list_s m_ports;      // 'ports' backing field
        list_si m_services;  // Service info

    public:  /* Constructors & Destructor */
        Socket();
        Socket(const Socket &t_sock);
        Socket(const property_s &t_addr, const property_l &t_ports);

        virtual ~Socket();

    public:  /* Operators */
        Socket &operator=(const Socket &t_sock) noexcept;

    public:  /* Methods */
        static const bool valid_port(const string &t_port);
        static const bool valid_port(const vector_s &t_ports);

        static const int valid_ip(const string &t_addr);

        void connect();

    private:  /* Methods */
        void close(addrinfoW *t_aiptr);
        void error() const;
        void error(const int &t_err) const;
        void error(const string &t_arg) const;
        void error(const int &t_err, const string &arg) const;

        const bool valid_sock(const SOCKET &t_sock) const noexcept;

        const HostState connect(addrinfoW *t_aiptr,
                                char (&t_buffer)[BUFFER_SIZE],
                                const EndPoint &t_ep);

        const int get_error() const;

        const int select(fd_set *t_rfds_ptr,
                         fd_set *t_wfds_ptr,
                         const timeval &t_to = { 0, 1 }) const;

        const int set_blocking(const bool &t_do_block);

        addrinfoW *startup(SvcInfo &t_si, const string &t_port);

        SvcInfo &update_svc(SvcInfo &t_si, const string &t_port,
                                           const HostState &t_hs) const;
    };
}

#endif // !SOCKET_H
