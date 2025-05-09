/*
* @file
*     tls_scanner.cpp
* @brief
*     Source file for an IPv4 network scanner with SSL/TLS capabilities.
*/
#include <memory>
#include <utility>
#include <boost/asio/io_context.hpp>
#include "includes/errors/arg_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/inet/scanners/tls_scanner.h"
#include "includes/threading/task_status.h"
#include "includes/threading/thread_pool.h"

/**
* @brief
*     Initialize the object.
*/
scan::TlsScanner::TlsScanner(TlsScanner&& t_scanner) noexcept
    : TcpScanner{t_scanner.m_io_ctx, t_scanner.m_args_ap.load()}
{
    *this = std::move(t_scanner);
}

/**
* @brief
*     Initialize the object.
*/
scan::TlsScanner::TlsScanner(io_context_t& t_io_ctx, shared_ptr<Args> t_argsp)
    : TcpScanner{t_io_ctx, t_argsp}
{
}

/**
* @brief
*     Create a new port scan task and submit it to
*     the underlying thread pool for execution.
*/
void scan::TlsScanner::post_port_scan(port_t t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{INVALID_PORTS_MSG, "t_port"};
    }

    if (!target.valid())
    {
        throw RuntimeEx{INVALID_TARGET_MSG, "TlsScanner::post_port_scan"};
    }

    // Post a new scan task to the thread pool
    m_pool.post([this, t_port]() mutable -> void
    {
        print_progress();
        set_status(t_port, TaskStatus::executing);

        io_context_t io_ctx;
        tls_client_ptr_t tls_clientp;

        client_ptr_t clientp{std::make_unique<TcpClient>(io_ctx, m_args_ap, m_rc_ap)};
        clientp->connect(t_port);

        if (clientp->is_connected())
        {
            bool success{process_data(clientp)};
            tls_clientp = std::make_unique<TlsClient>(io_ctx, m_args_ap, m_rc_ap);

            // Try to establish SSL/TLS connection
            if (!success)
            {
                tls_clientp->svcinfo() = clientp->svcinfo();

                clientp->disconnect();
                tls_clientp->connect(t_port);

                // SSL/TLS connection established
                if (tls_clientp->is_connected())
                {
                    process_data(tls_clientp);
                    tls_clientp->disconnect();
                }

                add_service(tls_clientp->svcinfo());
                tls_clientp->close();
            }
            else  // No SSL/TLS connection necessary
            {
                add_service(clientp->svcinfo());
                clientp->disconnect();
            }
        }
        else  // Failed to establish connection
        {
            add_service(clientp->svcinfo());
        }

        set_status(t_port, TaskStatus::complete);
        clientp->close();
    });
}
