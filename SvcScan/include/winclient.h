#pragma once

#ifndef WINCLIENT_H
#define WINCLIENT_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <string>
#include <winsock2.h>
#include "client.h"
#include "property.h"

#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#ifndef WIN_OS
#  define WIN_OS // Windows OS
#endif // !WIN_OS

namespace Scan
{
    /// ***
    /// Windows IPv4 TCP network socket client
    /// ***
    class WinClient : public Client
    {
    private: /* Constants */
        static constexpr ushort SOCKV = {(2 << 8) | 2}; // WSA version

        static constexpr int SHUT_RDWR = {SD_BOTH}; // Halt communication
        static constexpr int WSAENSLOOKUP = {11001}; // WSA DNS error

    public: /* Constructors & Destructor */
        WinClient();
        WinClient(const WinClient &wc);

        WinClient(const Property<std::string> &addr,
                  const Property<vector_s> &ports);

        virtual ~WinClient();

    public: /* Operators */
        WinClient &operator=(const WinClient &client);

    public: /* Methods */
        void start() override;
        const int valid_ip(const std::string &addr) const override;

    private: /* Methods */
        void close_sock(SOCKET &sock) const override;
        void wsa_error() const;
        void wsa_error(const int &error) const;
        void wsa_error(const std::string &arg) const;
        void wsa_errorf(const int &error, std::string &arg) const;

        template<size_t N>
        const int set_sockopts(SOCKET &sock, const int (&opts)[N]) const;

        const int sock_ioctl(SOCKET &sock, const bool &block) const override;
        const int sock_select(fd_set *rfds_ptr, fd_set *wfds_ptr);

        const std::string utf8(const std::wstring &data_w) const;
        const std::wstring utf16(const std::string &data) const;

        addrinfoW *prep_sock(SOCKET &sock, const std::string &port) const;
    };
}


#endif // !WINCLIENT_H
