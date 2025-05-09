/*
* @file
*     tcp_scanner.h
* @brief
*     Header file for an IPv4 TCP network scanner.
*/
#pragma once

#ifndef SCAN_TCP_SCANNER_H
#define SCAN_TCP_SCANNER_H

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
        using client_ptr_t      = unique_ptr<TcpClient>;
        using port_status_map_t = map<port_t, TaskStatus>;

    public:  /* Fields */
        atomic_bool out_json;  // Output results as JSON
        atomic_bool verbose;   // Enable verbose output

        string out_path;       // Output file path

        Hostname target;       // Target hostname
        List<port_t> ports;    // Target ports

    protected:  /* Fields */
        atomic_ptr_t<Args> m_args_ap;  // Command-line arguments atomic pointer
        atomic_ptr_t<TextRc> m_rc_ap;  // Embedded CSV resource atomic pointer

        Timeout m_timeout;             // Connection timeout

        io_context_t& m_io_ctx;        // I/O context reference
        Timer m_timer;                 // Scan duration timer

        string m_uri;                  // HTTP request URI
        ThreadPool m_pool;             // Execution thread pool

        mutable mutex m_ports_mtx;     // Port list mutex
        mutable mutex m_services_mtx;  // Service information list mutex
        mutable mutex m_statuses_mtx;  // Task execution status map mutex

        port_status_map_t m_statuses;  // Task execution status map
        List<SvcInfo> m_services;      // Service information list

    public:  /* Constructors & Destructor */
        TcpScanner() = delete;
        TcpScanner(const TcpScanner&) = delete;
        TcpScanner(TcpScanner&& t_scanner) noexcept;
        TcpScanner(io_context_t& t_io_ctx, shared_ptr<Args> t_argsp);

        virtual ~TcpScanner() = default;

    public:  /* Operators */
        TcpScanner& operator=(const TcpScanner&) = delete;
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

        client_ptr_t& process_data(client_ptr_t& t_clientp);

        ClientPtr auto& probe_http(ClientPtr auto& t_clientp);

        string json_report(const SvcTable& t_table,
                           bool t_colorize = false,
                           bool t_include_title = false) const;

        string scan_progress() const;
        string scan_summary(bool t_colorize = false, bool t_include_cmd = false) const;
    };
}

/**
* @brief
*     Perform HTTP communications to identify the server information.
*/
inline scan::ClientPtr auto& scan::TcpScanner::probe_http(ClientPtr auto& t_clientp)
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
