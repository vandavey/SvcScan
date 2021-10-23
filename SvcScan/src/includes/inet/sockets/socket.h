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
#include <vector>
#include <winsock2.h>
#include "../../conio/stdutil.h"
#include "../../containers/generic/list.h"
#include "../../except/nullptrex.h"
#include "../../filesys/filestream.h"
#include "../../properties/autoprop.h"
#include "../../utils/timer.h"
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

        using fstream = FileStream::fstream;
        using net     = NetUtil;
        using stdu    = StdUtil;
        using string  = std::string;

        using list_s   = List<string>;
        using list_si  = List<SvcInfo>;
        using list_ui  = List<uint>;
        using vector_s = std::vector<string>;

    private:  /* Constants */
        static constexpr int BUFFER_SIZE{ 1024 };    // Socket buffer size
        static constexpr int SHUT_RD{ SD_RECEIVE };  // Halt communication

    public:  /* Fields */
        static AutoProp<string> out_path;  // Output file path

        Property<string> addr;             // Target address
        Property<list_ui> ports;           // Target ports

    private:  /* Fields */
        static timeval m_timeout;  // Connection timeout

        SOCKET m_sock;             // Underlying socket
        string m_addr;             // 'addr' backing field

        Timer m_timer;             // Scan duration timer

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

        void connect();

    private:  /* Methods */
        void close(addrinfoW *t_aiptr);

        void show_progress(const uint &t_next_port,
                           const size_t &t_start_pos,
                           const bool &t_first_port) const;

        HostState connect(addrinfoW *t_aiptr,
                          char (&t_buffer)[BUFFER_SIZE],
                          const EndPoint &t_ep);

        HostState recv(char (&t_buffer)[BUFFER_SIZE]);

        int select(fd_set *t_rfds_ptr,
                   fd_set *t_wfds_ptr,
                   const timeval &t_to) const;

        int set_blocking(const bool &t_do_block);

        addrinfoW *startup(SvcInfo &t_si, const uint &t_port);
    };
}

#endif // !SOCKET_H
