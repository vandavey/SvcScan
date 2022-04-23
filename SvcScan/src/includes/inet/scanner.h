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

#include "../containers/svctable.h"
#include "../filesys/filestream.h"
#include "sockets/tcpclient.h"

namespace
{
    namespace error = boost::asio::error;
}

namespace scan
{
    /// ***
    /// IPv4 TCP and HTTP network scanner
    /// ***
    class Scanner final : public IArgsParser
    {
    private:  /* Type Aliases */
        using uint  = unsigned int;

        using error_code = boost::system::error_code;
        using fstream    = FileStream::fstream;
        using io_context = boost::asio::io_context;
        using net        = NetUtil;
        using parser     = ArgParser;
        using stdu       = StdUtil;
        using string     = std::string;
        using vector_s   = std::vector<string>;

    public:  /* Fields */
        bool verbose;      // Verbose output

        Hostname target;   // Target address
        string out_path;   // Output file path

        List<uint> ports;  // Target ports

    private:  /* Fields */
        string m_http_uri;         // HTTP request URI
        Args m_args;               // Cmd-line arguments

        Timeout m_conn_timeout;    // Connection timeout
        Timer m_timer;             // Scan duration timer

        io_context &m_ioc;         // I/O context reference
        TcpClient m_client;        // TCP network client

        List<SvcInfo> m_services;  // Service info

    public:  /* Constructors & Destructor */
        Scanner() = delete;
        Scanner(Scanner &&t_scanner) noexcept;
        Scanner(io_context &t_ioc, const Args &t_args);

        virtual ~Scanner();

    public:  /* Methods */
        void connect_timeout(const Timeout &t_timeout);
        void scan();

    private:  /* Methods */
        void close();
        void parse_args(const Args &t_args) override;
        void process_data();

        void save_results(const string &t_path,
                          const string &t_summary,
                          const SvcTable &t_table);

        void scan_port(const uint &t_port);

        string scan_progress(const uint &t_next_port,
                             const size_t &t_start_pos) const;

        string scan_summary() const;

        void show_progress(const uint &t_next_port,
                           const size_t &t_start_pos,
                           const bool &t_first) const;
    };
}

#endif // !SCANNER_H
