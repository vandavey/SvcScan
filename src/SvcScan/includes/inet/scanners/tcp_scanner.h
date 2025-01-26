/*
* @file
*     tcp_scanner.h
* @brief
*     Header file for an IPv4 TCP network scanner.
*/
#pragma once

#ifndef SCAN_TCP_SCANNER_H
#define SCAN_TCP_SCANNER_H

#include <map>
#include <boost/beast/http/verb.hpp>
#include "../../concepts/socket_concepts.h"
#include "../../console/args.h"
#include "../../errors/error_const_defs.h"
#include "../../errors/logic_ex.h"
#include "../../ranges/algo.h"
#include "../../ranges/list.h"
#include "../../resources/text_rc.h"
#include "../../threading/task_status.h"
#include "../../threading/thread_aliases.h"
#include "../../threading/thread_pool.h"
#include "../../utils/aliases.h"
#include "../../utils/timer.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../net_aliases.h"
#include "../services/svc_info.h"
#include "../services/svc_table.h"
#include "../sockets/hostname.h"
#include "../sockets/tcp_client.h"
#include "../sockets/timeout.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 TCP and HTTP network scanner.
    */
    class TcpScanner
    {
    protected:  /* Type Aliases */
        using client_ptr = unique_ptr<TcpClient>;
        using status_map = map<port_t, TaskStatus>;
        using status_t   = status_map::value_type;

    public:  /* Fields */
        atomic_bool out_json;  // Output results as JSON
        atomic_bool verbose;   // Enable verbose output

        string out_path;       // Output file path

        Hostname target;       // Target hostname
        List<port_t> ports;    // Target ports

    protected:  /* Fields */
        // Command-line arguments atomic shared pointer
        atomic_shared_ptr<Args> m_args_ap;

        // Embedded CSV resource atomic shared pointer
        atomic_shared_ptr<TextRc> m_trc_ap;

        io_context& m_ioc;             // I/O context reference

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
        TcpScanner(const TcpScanner&) = delete;
        TcpScanner(TcpScanner&& t_scanner) noexcept;
        TcpScanner(io_context& t_ioc, shared_ptr<Args> t_argsp);

        virtual ~TcpScanner() = default;

    public:  /* Operators */
        TcpScanner& operator=(const TcpScanner&) = default;
        TcpScanner& operator=(TcpScanner&& t_scanner) noexcept;

    public:  /* Methods */
        void scan();
        void wait();

    protected:  /* Methods */
        void add_service(const SvcInfo& t_info);
        void parse_argsp(shared_ptr<Args> t_argsp);
        virtual void post_port_scan(port_t t_port);
        void print_progress() const;
        void print_report(const SvcTable& t_table) const;
        void save_report(const SvcTable& t_table) const;
        void scan_shutdown();
        void scan_startup();
        void set_status(port_t t_port, TaskStatus t_status);

        size_t completed_tasks() const;

        double calc_progress() const;
        double calc_progress(size_t& t_completed) const;

        client_ptr& process_data(client_ptr& t_clientp);

        NetClientPtr auto& probe_http(NetClientPtr auto& t_clientp);

        string json_report(const SvcTable& t_table,
                           bool t_colorize = false,
                           bool t_inc_title = false) const;

        string scan_progress() const;
        string scan_summary(bool t_colorize = false, bool t_inc_cmd = false) const;
    };
}

/**
* @brief
*     Perform HTTP communications to identify the server information.
*/
inline scan::NetClientPtr auto& scan::TcpScanner::probe_http(NetClientPtr auto& t_clientp)
{
    if (!t_clientp->is_connected())
    {
        throw LogicEx{CLIENT_DISCONNECTED_MSG, "TcpScanner::probe_http"};
    }

    SvcInfo& svc_info{t_clientp->svcinfo()};
    const verb_t method{m_args_ap.load()->curl ? verb_t::get : verb_t::head};

    const Request<> request{method, target, m_uri};
    const Response<> response{t_clientp->request(request)};

    // Update HTTP service information
    if (response.valid())
    {
        const vector<string> replacement_subs{"_", "/"};

        svc_info.service = algo::fstr("http (%)", response.httpv.num_str());
        svc_info.summary = algo::replace(response.server(), replacement_subs, " ");
        svc_info.request = request;
        svc_info.response = response;
    }
    return t_clientp;
}

#endif // !SCAN_TCP_SCANNER_H
