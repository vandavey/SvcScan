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

#include "../../containers/svc_table.h"
#include "../../filesys/file_stream.h"
#include "../sockets/tcp_client.h"

namespace scan
{
    namespace
    {
        namespace error = boost::asio::error;
    }

    /**
    * @brief  IPv4 TCP and HTTP network scanner.
    */
    class Scanner : public IArgsParser
    {
    protected:  /* Type Aliases */
        using uint  = unsigned int;

        using error_code    = boost::system::error_code;
        using fstream       = FileStream::fstream;
        using io_context    = boost::asio::io_context;
        using net           = NetUtil;
        using port_iterator = List<uint>::iterator;
        using stdu          = StdUtil;
        using string        = std::string;

        template<class T>
        using unique_ptr = std::unique_ptr<T>;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Fields */
        bool verbose;      // Verbose output
        string out_path;   // Output file path

        Hostname target;   // Target address
        List<uint> ports;  // Target ports

    protected:  /* Fields */
        unique_ptr<TcpClient> m_clientp;  // TCP client smart pointer

        io_context &m_ioc;                // I/O context reference

        Timeout m_conn_timeout;           // Connection timeout
        Timer m_timer;                    // Scan duration timer

        string m_http_uri;                // HTTP request URI

        Args m_args;                      // Command-line arguments
        TextRc m_csv_rc;                  // Embedded CSV resource

        List<SvcInfo> m_services;         // Service info

    public:  /* Constructors & Destructor */
        Scanner() = delete;
        Scanner(const Scanner &) = delete;
        Scanner(Scanner &&t_scanner) noexcept;
        Scanner(io_context &t_ioc, const Args &t_args);

        virtual ~Scanner();

    public:  /* Operators */
        Scanner &operator=(const Scanner &) = default;
        Scanner &operator=(Scanner &&t_scanner) noexcept;

    public:  /* Methods */
        virtual void close();
        void connect_timeout(const Timeout &t_timeout);
        void scan();

    protected:  /* Methods */
        void parse_args(const Args &t_args) override;
        void probe_http(SvcInfo &t_si, HostState &t_hs);

        void save_report(const string &t_path,
                         const string &t_summary,
                         const SvcTable &t_table);

        virtual void scan_port(const uint &t_port);
        void show_progress(const port_iterator &t_port_it) const;
        void startup();

        virtual bool process_data();

        string progress(const port_iterator &t_port_it) const;
        string summary() const;
    };
}

#endif // !SCANNER_H
