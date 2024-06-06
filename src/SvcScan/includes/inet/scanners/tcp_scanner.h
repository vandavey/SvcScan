/*
* @file
*     tcp_scanner.h
* @brief
*     Header file for an IPv4 TCP network scanner.
*/
#pragma once

#ifndef SCAN_TCP_SCANNER_H
#define SCAN_TCP_SCANNER_H

#include "../../concepts/socket_concepts.h"
#include "../../containers/svc_table.h"
#include "../../errors/null_ptr_ex.h"
#include "../../io/filesys/file_stream.h"
#include "../../threading/task_status.h"
#include "../../threading/thread_defs.h"
#include "../../threading/thread_pool.h"
#include "../../utils/json_util.h"
#include "../../utils/type_defs.h"
#include "../sockets/tcp_client.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 TCP and HTTP network scanner.
    */
    class TcpScanner : public IArgsParser
    {
    protected:  /* Type Aliases */
        using algo         = Algorithm;
        using client_ptr   = unique_ptr<TcpClient>;
        using json         = JsonUtil;
        using json_value_t = boost::json::value;
        using net          = NetUtil;
        using status_map   = map<port_t, TaskStatus>;
        using status_t     = status_map::value_type;
        using stdu         = StdUtil;

    private:  /* Type Aliases */
        using this_t = TcpScanner;

    public:  /* Fields */
        atomic_bool out_json;  // Output results as JSON
        atomic_bool verbose;   // Enable verbose output

        string out_path;       // Output file path

        Hostname target;       // Target hostname
        List<port_t> ports;    // Target ports

    protected:  /* Fields */
        uint_t m_threads;              // Thread pool thread count

        atomic_ptr<Args> m_args_ap;    // Command-line arguments atomic smart pointer
        atomic_ptr<TextRc> m_trc_ap;   // Embedded CSV resource atomic smart pointer

        io_context &m_ioc;             // I/O context reference

        Timeout m_conn_timeout;        // Connection timeout
        Timer m_timer;                 // Scan duration timer

        string m_uri;                  // HTTP request URI
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
        virtual void post_port_scan(const port_t &t_port);
        void print_progress() const;
        void print_report(const SvcTable &t_table) const;
        void save_report(const SvcTable &t_table) const;
        void scan_shutdown();
        void scan_startup();
        void set_status(const port_t &t_port, const TaskStatus &t_status);

        size_t completed_tasks() const;

        double calc_progress() const;
        double calc_progress(size_t &t_completed) const;

        client_ptr &&process_data(client_ptr &&t_clientp);

        template<NetClientPtr T>
        T &&probe_http(T &&t_clientp, HostState &t_state);

        string json_report(const SvcTable &t_table,
                           const bool &t_colorize = false,
                           const bool &t_inc_title = false) const;

        string scan_progress() const;

        string scan_summary(const bool &t_colorize = false,
                            const bool &t_inc_cmd = false) const;
    };
}

/**
* @brief
*     Perform HTTP communications to identify the server information.
*/
template<scan::NetClientPtr T>
inline T &&scan::TcpScanner::probe_http(T &&t_clientp, HostState &t_state)
{
    if (!t_clientp->is_connected())
    {
        throw LogicEx{ "TcpScanner::probe_http", "TCP client must be connected" };
    }

    SvcInfo &svc_info{ t_clientp->svcinfo() };
    const verb_t method{ m_args_ap.load()->curl ? verb_t::get : verb_t::head };

    const Request<> request{ method, target, m_uri };
    const Response<> response{ t_clientp->request(request) };

    // Update HTTP service information
    if (response.valid())
    {
        t_state = HostState::open;

        svc_info.service = algo::fstr("http (%)", response.httpv.num_str());
        svc_info.summary = algo::replace(response.server(), { "_", "/" }, " ");
        svc_info.request = request;
        svc_info.response = response;
    }
    return std::forward<T>(t_clientp);
}

#endif // !SCAN_TCP_SCANNER_H
