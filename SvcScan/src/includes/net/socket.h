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

namespace Scan
{
    /// ***
    /// Windows IPv4 TCP network socket wrapper
    /// ***
    class Socket
    {
    private:  /* Types & Constants */
        using llong = long long;
        using uchar = unsigned char;
        using ulong = unsigned long;
        using ushort = unsigned short;

        using string = std::string;
        using list_s = List<string>;
        using list_si = List<SvcInfo>;
        using vector_s = std::vector<string>;
        using vector_ul = std::vector<ulong>;

        using property_l = Property<list_s>;
        using property_s = Property<string>;

        static constexpr ushort SOCKV = {(2 << 8) | 2};  // WSA version
        static constexpr int BUFFER_SIZE = {1024};       // Socket buffer size
        static constexpr int SHUT_RD = {SD_RECEIVE};     // Halt communication

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
        Socket(const Socket &sock);
        Socket(const property_s &addr, const property_l &ports);

        virtual ~Socket();

    public:  /* Operators */
        Socket &operator=(const Socket &sock) noexcept;

    public:  /* Methods */
        static const bool valid_port(const string &port);
        static const bool valid_port(const vector_s &ports);

        static const int valid_ip(const string &addr);

        void connect();

    private:  /* Methods */
        void close(addrinfoW *ai_ptr);
        void error() const;
        void error(const int &err) const;
        void error(const string &arg) const;
        void error(const int &err, const string &arg) const;

        const bool valid_sock(const SOCKET &sock) const noexcept;

        const HostState connect(addrinfoW *ai_ptr, char (&buffer)[BUFFER_SIZE],
                                                   const EndPoint &ep);

        const int get_error() const;

        template<size_t N>
        const int set_sockopts(const int (&sock_opts)[N]);

        const int select(fd_set *rfds_ptr, fd_set *wfds_ptr,
                                           const timeval &to = {0, 1}) const;

        const int set_blocking(const bool &block);

        addrinfoW *startup(SvcInfo &si, const string &port);

        SvcInfo &update_svc(SvcInfo &si, const string &port,
                                         const HostState &hs) const;
    };
}

/// ***
/// Apply socket options on the socket descriptor
/// ***
template<size_t N>
inline const int Scan::Socket::set_sockopts(const int (&sock_opts)[N])
{
    if (m_sock == NULL)
    {
        throw NullArgEx("sock");
    }

    // Invalid socket descriptor
    if (!valid_sock(m_sock))
    {
        throw ArgEx("sock", "Invalid socket descriptor");
    }

    int code = {SOCKET_ERROR};
    const char *ptr = {Util::itoc(3500)};

    if (ptr == nullptr)
    {
        throw NullPtrEx("ptr");
    }
    const int len = {static_cast<int>(sizeof(llong))};

    // Set socket options
    for (const int &opt : sock_opts)
    {
        if (opt == NULL)
        {
            throw NullArgEx("*opts");
        }
        code = ::setsockopt(m_sock, SOL_SOCKET, opt, ptr, len);

        // Failed to set socket option
        if (code != static_cast<int>(NO_ERROR))
        {
            Util::errorf("Failed to set sockopt: '%'", Util::itos(opt));
        }
    }
    return code;
}

#endif // !SOCKET_H
