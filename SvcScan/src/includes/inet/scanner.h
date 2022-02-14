/*
*  scanner.h
*  ---------
*  Header file for an IPv4 network scanner
*/
#pragma once

#ifndef SCANNER_H
#define SCANNER_H

#ifndef UNICODE
#  define UNICODE 1
#endif // !UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <sstream>
#include <string>
#include "../filesys/filestream.h"
#include "../io/stdutil.h"
#include "../utils/argparser.h"
#include "sockets/socket.h"

namespace scan
{
    /// ***
    /// IPv4 TCP and HTTP network scanner
    /// ***
    class Scanner final
    {
    private:  /* Type Aliases */
        using uchar = unsigned char;
        using uint  = unsigned int;

        using fstream = FileStream::fstream;
        using net     = NetUtil;
        using parser  = ArgParser;
        using sstream = std::stringstream;
        using stdu    = StdUtil;
        using string  = std::string;

        using list_s   = List<string>;
        using list_si  = List<SvcInfo>;
        using list_ui  = List<uint>;
        using vector_s = std::vector<string>;

    public:  /* Fields */
        bool verbose;     // Verbose output

        string target;    // Target address
        string out_path;  // Output file path

        list_ui ports;    // Target ports

    private:  /* Fields */
        Socket m_sock;           // TCP network socket

        string m_http_uri;       // HTTP request URI
        Args m_args;             // Cmd-line arguments

        Timeout m_conn_timeout;  // Connection timeout
        Timer m_timer;           // Scan duration timer

        list_si m_services;      // Service info

    public:  /* Constructors & Destructor */
        Scanner(const Scanner &t_scanner);
        Scanner(const Args &t_args);

        virtual ~Scanner();

    private:  /* Constructors (deleted) */
        Scanner() = delete;

    public:  /* Operators */
        Scanner &operator=(const Scanner &t_scanner);

    public:  /* Methods */
        void connect_timeout(const Timeout &t_timeout);
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
