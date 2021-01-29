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
#include "../util.h"
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

        using vector_s = std::vector<std::string>;
        using list_s = List<std::string>;
        using list_si = List<SvcInfo>;

        static constexpr ushort SOCKV = {(2 << 8) | 2};  // WSA version
        static constexpr int BUFFERSIZE = {1024};        // Socket buffer size
        static constexpr int SHUT_RDWR = {SD_BOTH};      // Halt communication
        static constexpr int WSAENSLOOKUP = {11001};     // WSA DNS error

    public:  /* Fields */
        Property<std::string> addr;  // Target address
        Property<list_s> ports;      // Target ports

    protected:  /* Fields */
        std::string m_addr;  // Addr backing field
        list_s m_ports;      // Ports backing field
        list_si m_services;  // Service info

    public:  /* Constructors & Destructor */
        Socket() = default;
        explicit Socket(const Socket &sock);

        Socket(const Property<std::string> &addr,
               const Property<list_s> &ports);

        virtual ~Socket();

    public:  /* Operators */
        Socket &operator=(const Socket &sock) noexcept;

    public:  /* Methods */
        static const int valid_ip(const std::string &addr);
        static const bool valid_port(const std::string &port);
        static const bool valid_port(const vector_s &ports);

        void connect();

    private:  /* Methods */
        void close(SOCKET &sock) const;
        void error() const;
        void error(const int &err) const;
        void error(const std::string &arg) const;
        void error(const int &err, std::string &arg) const;

        const bool valid_sock(const SOCKET &sock) const noexcept;

        const int ioctl(SOCKET &sock, const bool &block) const;
        const int select(fd_set *rfds_ptr, fd_set *wfds_ptr) const;

        template<size_t N>
        const int setsockopts(SOCKET &sock, const int (&opts)[N]) const;

        addrinfoW *startup(SOCKET &sock, const std::string &port) const;

        Socket &swap(const Property<std::string> &addr,
                     const Property<list_s> &ports);
    };
}

/// ***
/// Apply socket options on the socket descriptor
/// ***
template<size_t N>
inline const int Scan::Socket::setsockopts(SOCKET &sock,
                                           const int (&opts)[N]) const {
    if (sock == static_cast<SOCKET>(NULL))
    {
        throw NullArgEx("sock");
    }

    if (!valid_sock(sock))
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
    for (const int &opt : opts)
    {
        if (opt == NULL)
        {
            throw NullArgEx("*opts");
        }
        code = ::setsockopt(sock, SOL_SOCKET, opt, ptr, len);

        // Failed to set socket option
        if (code != static_cast<int>(NO_ERROR))
        {
            Util::errorf("Failed to set sockopt: '%'", Util::itos(opt));
        }
    }
    return code;
}

#endif // !SOCKET_H