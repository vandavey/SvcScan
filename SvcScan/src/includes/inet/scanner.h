/*
*  scanner.h
*  ---------
*  Header file for an IPv4 network scanner
*/
#pragma once

#ifndef SCANNER_H
#define SCANNER_H

#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <string>
#include "../conio/stdutil.h"
#include "../filesys/filestream.h"
#include "sockets/socket.h"

namespace scan
{
    /// ***
    /// IPv4 TCP and HTTP network scanner
    /// ***
    class Scanner final
    {
    private:  /* Types */
        using uint = unsigned int;

        using fstream = FileStream::fstream;
        using net     = NetUtil;
        using parser  = ArgParser;
        using stdu    = StdUtil;
        using string  = std::string;

        using list_s   = List<string>;
        using list_si  = List<SvcInfo>;
        using list_ui  = List<uint>;
        using vector_s = std::vector<string>;

    public:  /* Fields */
        static AutoProp<string> out_path;  // Output file path

        Property<string> target;           // Target address
        Property<list_ui> ports;           // Target ports

    private:  /* Fields */
        static Timeout m_conn_timeout;  // Connection timeout

        Socket m_sock;                  // TCP network socket
        Timer m_timer;                  // Scan duration timer

        string m_target;                // 'target' backing field

        list_ui m_ports;                // 'ports' backing field
        list_si m_services;             // Service info

    public:  /* Constructors & Destructor */
        Scanner(const Scanner &t_scanner);
        Scanner(const string &t_target, const list_ui &t_ports);

        virtual ~Scanner();

    private:  /* Constructors */
        Scanner();

    public:  /* Operators */
        Scanner &operator=(const Scanner &t_scanner);

    public:  /* Methods */
        static void connect_timeout(const Timeout &t_timeout);

        void scan();

    private:  /* Methods */
        void close();
        void process_data(const bool &t_close_sock = false);
        void scan_port(const uint &t_port);

        void show_progress(const uint &t_next_port,
                           const size_t &t_start_pos,
                           const bool &t_first) const;
    };
}

#endif // !SCANNER_H
