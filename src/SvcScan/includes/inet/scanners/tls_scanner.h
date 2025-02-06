/*
* @file
*     tls_scanner.h
* @brief
*     Header file for an IPv4 network scanner with SSL/TLS capabilities.
*/
#pragma once

#ifndef SCAN_TLS_SCANNER_H
#define SCAN_TLS_SCANNER_H

#include <string>
#include "../../concepts/concepts.h"
#include "../../concepts/socket_concepts.h"
#include "../../console/args.h"
#include "../../errors/error_const_defs.h"
#include "../../errors/logic_ex.h"
#include "../../errors/null_ptr_ex.h"
#include "../../utils/aliases.h"
#include "../../utils/const_defs.h"
#include "../net.h"
#include "../net_aliases.h"
#include "../services/svc_info.h"
#include "../sockets/host_state.h"
#include "../sockets/tcp_client.h"
#include "../sockets/tls_client.h"
#include "tcp_scanner.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 TCP and HTTP network scanner with SSL/TLS capabilities.
    */
    class TlsScanner final : public TcpScanner
    {
    private:  /* Type Aliases */
        using tls_client_ptr = unique_ptr<TlsClient>;

    public:  /* Constructors & Destructor */
        TlsScanner() = delete;
        TlsScanner(const TlsScanner&) = delete;
        TlsScanner(TlsScanner&& t_scanner) noexcept;
        TlsScanner(io_context& t_ioc, shared_ptr<Args> t_argsp);

        virtual ~TlsScanner() = default;

    public:  /* Operators */
        TlsScanner& operator=(const TlsScanner&) = delete;
        TlsScanner& operator=(TlsScanner&& t_scanner) noexcept;

    private:  /* Methods */
        void post_port_scan(port_t t_port) override;

        bool process_data(NetClientPtr auto& t_clientp);
    };
}

/**
* @brief
*     Process the inbound and outbound socket stream data.
*/
inline bool scan::TlsScanner::process_data(NetClientPtr auto& t_clientp)
{
    if (t_clientp == nullptr)
    {
        throw NullPtrEx{"t_clientp"};
    }

    if (!t_clientp->is_connected())
    {
        throw LogicEx{CLIENT_DISCONNECTED_MSG, "TlsScanner::process_data"};
    }
    bool success{true};

    TlsClient::buffer_t buffer{CHAR_NULL};
    SvcInfo& svc_info{t_clientp->svcinfo()};

    const size_t num_read{t_clientp->recv(buffer)};
    HostState state{t_clientp->host_state()};

    // Parse banner or probe HTTP information
    if (state == HostState::open)
    {
        const string recv_data(&buffer[0], num_read);

        if (!recv_data.empty())
        {
            svc_info.parse(recv_data);
            net::update_svc(*m_trc_ap.load(), svc_info, state);
        }

        if (recv_data.empty() || m_args_ap.load()->curl)
        {
            probe_http(t_clientp);
            success = !svc_info.summary.empty();

            if (success && SmartPtrOfType<decltype(t_clientp), TlsClient>)
            {
                svc_info.service = algo::replace(svc_info.service, "http", "https");
            }
        }
    }
    net::update_svc(*m_trc_ap.load(), svc_info, state);

    return success;
}

#endif // !SCAN_TLS_SCANNER_H
