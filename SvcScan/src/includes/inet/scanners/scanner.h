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

#include <mutex>
#include <sdkddkver.h>
#include "../../concepts/socket_concepts.h"
#include "../../containers/svc_table.h"
#include "../../except/null_ptr_ex.h"
#include "../../filesys/file_stream.h"
#include "../../threading/task_status.h"
#include "../../threading/thread_pool.h"
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
        using uint = unsigned int;

        using error_code    = boost::system::error_code;
        using fstream       = FileStream::fstream;
        using io_context    = boost::asio::io_context;
        using mutex         = std::mutex;
        using net           = NetUtil;
        using status_map    = std::map<uint, TaskStatus>;
        using status_t      = status_map::value_type;
        using stdu          = StdUtil;
        using string        = std::string;

        template<class T>
        using atomic_ptr = std::atomic<std::shared_ptr<T>>;
        
        template<class T>
        using shared_ptr = std::shared_ptr<T>;

        template<class T>
        using unique_ptr = std::unique_ptr<T>;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Type Aliases */
        using this_t = Scanner;

    public:  /* Fields */
        bool verbose;      // Verbose output
        string out_path;   // Output file path

        Hostname target;   // Target address
        List<uint> ports;  // Target ports

    protected:  /* Fields */
        uint m_concurrency;              // Max concurrent connections

        atomic_ptr<Args> m_args_ap;      // Command-line arguments smart pointer
        atomic_ptr<TextRc> m_csv_rc_ap;  // Embedded CSV resource smart pointer

        io_context &m_ioc;               // I/O context reference

        Timeout m_conn_timeout;          // Connection timeout
        Timer m_timer;                   // Scan duration timer

        string m_http_uri;               // HTTP request URI
        ThreadPool m_pool;               // Execution thread pool

        mutable mutex m_kb_io_mutex;     // Keyboard I/O mutex
        mutable mutex m_list_mutex;      // Service list mutex
        mutable mutex m_map_mutex;       // Task status map mutex

        status_map m_status_map;         // Task execution status map
        List<SvcInfo> m_services;        // Service info

    public:  /* Constructors & Destructor */
        Scanner() = delete;
        Scanner(const Scanner &) = delete;
        Scanner(Scanner &&t_scanner) noexcept;
        Scanner(io_context &t_ioc, shared_ptr<Args> t_argsp);

        virtual ~Scanner() = default;

    public:  /* Operators */
        Scanner &operator=(const Scanner &) = default;
        Scanner &operator=(Scanner &&t_scanner) noexcept;

    public:  /* Methods */
        void connect_timeout(const Timeout &t_timeout);
        void scan();
        void wait();

    protected:  /* Methods */
        void add_service(const SvcInfo &t_si);
        void parse_argsp(shared_ptr<Args> t_argsp) override;
        virtual void post_port_scan(const uint &t_port);

        void save_report(const string &t_path,
                         const string &t_summary,
                         const SvcTable &t_table);

        void show_progress() const;
        void startup();
        void update_status(const uint &t_port, const TaskStatus &t_status);

        size_t completed_tasks() const;
        double calc_progress() const;

        string progress() const;
        string summary() const;

        template<NetClientPtr T>
        T &&probe_http(T &&t_clientp, HostState &t_hs);

        template<NetClientPtr T>
        T &&process_data(T &&t_clientp);

        template<NetClientPtr T>
        T &&process_data(T &&t_clientp, bool &t_success);
    };
}

/**
* @brief  Perform HTTP communications to identify server information.
*/
template<scan::NetClientPtr T>
inline T &&scan::Scanner::probe_http(T &&t_clientp, HostState &t_hs)
{
    if (!t_clientp->is_connected())
    {
        throw LogicEx{ "Scanner::probe_http", "TCP client must be connected" };
    }

    const Request request{ http::verb::head, target, m_http_uri };
    const Response response{ t_clientp->request(request) };

    // Update HTTP service information
    if (response.valid())
    {
        t_hs = HostState::open;
        SvcInfo &svc_info{ t_clientp->svcinfo() };

        svc_info.banner = Util::replace(response.server(),
                                        vector<string>{ "_", "/" },
                                        " ");

        svc_info.service = Util::fstr("http (%)", response.httpv.num_str());
        svc_info.summary = svc_info.banner;
    }
    return std::forward<T>(t_clientp);
}

/**
* @brief  Read and process the inbound socket stream data.
*/
template<scan::NetClientPtr T>
inline T &&scan::Scanner::process_data(T &&t_clientp)
{
    bool discard{ false };
    return process_data(std::forward<T>(t_clientp), discard);
}

/**
* @brief  Read and process the inbound socket stream data. Sets the boolean
*         reference to true if all data was successfully processed.
*/
template<scan::NetClientPtr T>
inline T &&scan::Scanner::process_data(T &&t_clientp, bool &t_success)
{
    if (t_clientp == nullptr)
    {
        throw NullPtrEx{ "t_clientp" };
    }

    if (!t_clientp->is_connected())
    {
        throw LogicEx{ "Scanner::process_data", "TCP client must be connected" };
    }

    t_success = true;
    char buffer[TcpClient::BUFFER_SIZE]{ '\0' };

    const size_t bytes_read{ t_clientp->recv(buffer) };
    HostState state{ t_clientp->host_state() };

    if (state == HostState::open)
    {
        const string recv_data{ std::string_view(&buffer[0], bytes_read) };

        // Probe HTTP version information
        if (recv_data.empty())
        {
            t_clientp = probe_http(std::move(t_clientp), state);

            if (!net::no_error(t_clientp->last_error()))
            {
                t_success = false;
            }
        }
        else  // Parse TCP banner data
        {
            t_clientp->svcinfo().parse(recv_data);
        }
    }

    net::update_svc(*m_csv_rc_ap.load(), t_clientp->svcinfo(), state);
    return std::forward<T>(t_clientp);
}

#endif // !SCANNER_H
