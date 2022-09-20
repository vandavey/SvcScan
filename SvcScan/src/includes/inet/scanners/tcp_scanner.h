/*
*  tcp_scanner.h
*  -------------
*  Header file for an IPv4 TCP network scanner
*/
#pragma once

#ifndef TCP_SCANNER_H
#define TCP_SCANNER_H

#ifndef UNICODE
#  define UNICODE 1
#endif // !UNICODE

#include <atomic>
#include <mutex>
#include <sdkddkver.h>
#include "../../concepts/socket_concepts.h"
#include "../../containers/svc_table.h"
#include "../../except/null_ptr_ex.h"
#include "../../io/filesys/file_stream.h"
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
    class TcpScanner : public IArgsParser
    {
    protected:  /* Type Aliases */
        using uint = unsigned int;

        using algo        = Algorithm;
        using atomic_bool = std::atomic_bool;
        using client_ptr  = std::unique_ptr<TcpClient>;
        using error_code  = boost::system::error_code;
        using fstream     = FileStream::fstream;
        using io_context  = boost::asio::io_context;
        using mutex       = std::mutex;
        using net         = NetUtil;
        using status_map  = std::map<uint, TaskStatus>;
        using status_t    = status_map::value_type;
        using stdu        = StdUtil;
        using string      = std::string;

        template<class T>
        using atomic_ptr = std::atomic<std::shared_ptr<T>>;

        template<class T>
        using shared_ptr = std::shared_ptr<T>;

        template<class T>
        using unique_ptr = std::unique_ptr<T>;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Type Aliases */
        using this_t = TcpScanner;

    public:  /* Fields */
        atomic_bool verbose;  // Enable verbose output
        string out_path;      // Output file path

        Hostname target;      // Target address
        List<uint> ports;     // Target ports

    protected:  /* Fields */
        uint m_threads;                // Thread pool thread count

        atomic_ptr<Args> m_args_ap;    // Command-line arguments smart pointer
        atomic_ptr<TextRc> m_trc_ap;   // Embedded CSV resource smart pointer

        io_context &m_ioc;             // I/O context reference

        Timeout m_conn_timeout;        // Connection timeout
        Timer m_timer;                 // Scan duration timer

        string m_http_uri;             // HTTP request URI
        ThreadPool m_pool;             // Execution thread pool

        mutable mutex m_ports_mtx;     // Port list mutex
        mutable mutex m_services_mtx;  // Service list mutex
        mutable mutex m_statuses_mtx;  // Task execution status map mutex

        status_map m_statuses;         // Task execution status map
        List<SvcInfo> m_services;      // Service info

    public:  /* Constructors & Destructor */
        TcpScanner() = delete;
        TcpScanner(const TcpScanner &) = delete;
        TcpScanner(TcpScanner &&t_scanner) noexcept;
        TcpScanner(io_context &t_ioc, shared_ptr<Args> t_argsp);

        virtual ~TcpScanner() = default;

    public:  /* Operators */
        TcpScanner &operator=(const TcpScanner &) = default;
        TcpScanner &operator=(TcpScanner &&t_scanner) noexcept;

    public:  /* Methods */
        void connect_timeout(const Timeout &t_timeout);
        void scan();
        void wait();

    protected:  /* Methods */
        void add_service(const SvcInfo &t_info);
        void parse_argsp(shared_ptr<Args> t_argsp) override;
        virtual void post_port_scan(const uint &t_port);

        void save_report(const string &t_path,
                         const string &t_summary,
                         const SvcTable &t_table);

        void scan_startup();
        void show_progress() const;
        void update_status(const uint &t_port, const TaskStatus &t_status);

        size_t completed_tasks() const;

        double calc_progress() const;
        double calc_progress(size_t &t_completed) const;

        client_ptr &&process_data(client_ptr &&t_clientp);

        template<NetClientPtr T>
        T &&probe_http(T &&t_clientp, HostState &t_state);

        string progress() const;
        string summary() const;
    };
}

/**
* @brief  Perform HTTP communications to identify the server version.
*/
template<scan::NetClientPtr T>
inline T &&scan::TcpScanner::probe_http(T &&t_clientp, HostState &t_state)
{
    if (!t_clientp->is_connected())
    {
        throw LogicEx{ "TcpScanner::probe_http", "TCP client must be connected" };
    }
    SvcInfo &svc_info{ t_clientp->svcinfo() };

    const Request<> request{ target, m_http_uri };
    const Response<> response{ t_clientp->request(request) };

    // Update HTTP service information
    if (response.valid())
    {
        t_state = HostState::open;
        svc_info.service = algo::fstr("http (%)", response.httpv.num_str());

        svc_info.summary = algo::replace(response.server(),
                                         vector<string>{ "_", "/" },
                                         " ");

        svc_info.req_headers = request.msg_headers();
        svc_info.req_httpv = request.httpv;
        svc_info.req_method = request.method();
        svc_info.req_uri = request.uri();

        svc_info.resp_headers = response.msg_headers();
        svc_info.resp_httpv = response.httpv;
        svc_info.resp_status = response.status();
    }
    return std::forward<T>(t_clientp);
}

#endif // !TCP_SCANNER_H
