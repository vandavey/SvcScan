/*
*  socket.h
*  --------
*  Header file for an IPv4 TCP network socket
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
#include <winsock2.h>
#include "../../utils/util.h"
#include "../netutil.h"
#include "../svcinfo.h"

namespace scan
{
    /// ***
    /// Windows IPv4 TCP network socket wrapper
    /// ***
    class Socket
    {
    private:  /* Types */
        using uint = unsigned int;

        using net    = NetUtil;
        using stdu   = StdUtil;
        using string = std::string;

    public:  /* Constants */
        static constexpr int BUFFER_SIZE{ 1024 };  // Socket buffer size

    private:  /* Constants */
        static constexpr int SHUT_RD{ SD_RECEIVE };  // Halt communication

    public:  /* Fields */
        Property<uint> port;    // Target port
        Property<string> addr;  // Target address

    private:  /* Fields */
        uint m_port;        // 'port' backing field
        SOCKET m_sock;      // Underlying socket

        timeval m_timeout;  // Connection timeout
        string m_addr;      // 'addr' backing field

        SvcInfo m_info;     // Service information

    public:  /* Constructors & Destructor */
        Socket();
        Socket(const Socket &t_sock);
        Socket(const string &t_addr);

        virtual ~Socket();

    public:  /* Operators */
        Socket &operator=(const Socket &t_sock) noexcept;
        Socket &operator=(const SOCKET &t_winsock) noexcept;

    public:  /* Methods */
        void close();
        void set_timeout(const uint &t_sec, const uint &t_ms);

        bool connect(const string &t_addr, const uint &t_port);
        bool valid() const noexcept;

        HostState recv(char (&t_buffer)[BUFFER_SIZE],
                       const timeval &t_to = { 1, 0 });

        SOCKET get_socket() noexcept;

        SvcInfo get_svcinfo() const noexcept;

    private:  /* Methods */
        void close(addrinfoW *t_aiptr);

        int select(fd_set *t_rfds_ptr,
                   fd_set *t_wfds_ptr,
                   const timeval &t_to) const;

        int set_blocking(const bool &t_do_block);

        addrinfoW *startup(SvcInfo &t_si, const uint &t_port);
    };
}

#endif // !SOCKET_H
