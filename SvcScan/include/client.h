#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include "property.h"
#include "svcinfo.h"
#include "style.h"

#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#ifdef _WIN32
#  ifndef WIN_OS
#    define WIN_OS // Windows OS
#  endif// !WIN_OS
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif// !WIN32_LEAN_AND_MEAN
#  include <winsock2.h>
#else
#  ifndef NIX_OS
#    define NIX_OS // Unix OS
#  endif// !NIX_OS
#  include <netinet/in.h>
#endif // _WIN32

namespace Scan
{
    /// ***
    /// Abstract base class for TCP socket client 
    /// ***
    class Client
    {
    protected: /* Constants & Types */
        typedef long long llong;
        typedef unsigned char uchar;
        typedef unsigned long ulong;
        typedef unsigned short ushort;

        typedef std::vector<std::string> vector_s;
        typedef std::vector<SvcInfo> vector_si;

        static constexpr int BUFFERSIZE = {1024};

    protected: /* Fields */
        vector_s m_ports; // Ports backing field
        vector_si m_services; // Service info

        std::string m_addr; // Addr backing field

    public: /* Properties */
        Property<std::string> addr; // Target address
        Property<vector_s> ports; // Target ports

    public: /* Constructors & Destructor */
        Client();
        Client(const Client &client);
        Client(const Property<std::string> &addr,
               const Property<vector_s> &ports);

        virtual ~Client();

    public: /* Operators */
        Client &operator=(const Client &client);

    public: /* Methods */
        virtual void start() = 0;
        virtual const bool valid_port(const std::string &port) const;
        virtual const int valid_ip(const std::string &addr) const = 0;

    protected: /* Methods */
        virtual void close_sock(SOCKET &sock) const = 0;

        virtual const int sock_ioctl(SOCKET &sock,
                                     const bool &block) const = 0;

        virtual const bool valid_sock(const SOCKET &sock) const;

        const char *itoc(const llong &num) const;
        const std::string itos(const llong &num) const;

        const Client &base() const;
        Client &swap(const Client &client);

        Client &swap(const Property<std::string> &addr,
                     const Property<vector_s> &ports);
    };
}

#endif // !CLIENT_H
