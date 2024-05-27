/*
* @file
*     tls_scanner.cpp
* @brief
*     Source file for an IPv4 network scanner with SSL/TLS capabilities.
*/
#include "includes/errors/null_ptr_ex.h"
#include "includes/inet/scanners/tls_scanner.h"
#include "includes/resources/resource.h"

/**
* @brief  Initialize the object.
*/
scan::TlsScanner::TlsScanner(TlsScanner &&t_scanner) noexcept
    : base_t(t_scanner.m_ioc, t_scanner.m_args_ap.load())
{
    *this = std::forward<this_t>(t_scanner);
}

/**
* @brief  Initialize the object.
*/
scan::TlsScanner::TlsScanner(io_context &t_ioc, shared_ptr<Args> t_argsp)
    : base_t(t_ioc, t_argsp)
{
}

/**
* @brief  Move assignment operator overload.
*/
scan::TlsScanner &scan::TlsScanner::operator=(TlsScanner &&t_scanner) noexcept
{
    if (this != &t_scanner)
    {
        std::scoped_lock lock{ m_ports_mtx, m_services_mtx, m_statuses_mtx };

        m_args_ap = t_scanner.m_args_ap.load();
        m_conn_timeout = t_scanner.m_conn_timeout;
        m_services = t_scanner.m_services;
        m_statuses = t_scanner.m_statuses;
        m_threads = t_scanner.m_threads;
        m_timer = t_scanner.m_timer;
        m_trc_ap = t_scanner.m_trc_ap.load();
        m_uri = t_scanner.m_uri;

        out_path = t_scanner.out_path;
        ports = t_scanner.ports;
        target = t_scanner.target;
        verbose = t_scanner.verbose.load();
    }
    return *this;
}

/**
* @brief  Create a new port scan task and submit it to the underlying
*         thread pool for execution.
*/
void scan::TlsScanner::post_port_scan(const port_t &t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number specified" };
    }

    if (!target.is_valid())
    {
        throw RuntimeEx{ "TlsScanner::post_port_scan", "Invalid underlying target" };
    }

    // Post a new scan task to the thread pool
    m_pool.post([&, this]() mutable -> void
    {
        print_progress();
        set_status(t_port, TaskStatus::executing);

        io_context ioc;
        tls_client_ptr tls_clientp;

        client_ptr clientp{ std::make_unique<TcpClient>(ioc, m_args_ap, m_trc_ap) };

        clientp->connect(t_port);

        if (clientp->is_connected())
        {
            bool success{ false };

            clientp = process_data(std::move(clientp), success);
            tls_clientp = std::make_unique<TlsClient>(ioc, m_args_ap, m_trc_ap);

            // Try establishing SSL/TLS connection
            if (!success)
            {
                tls_clientp->svcinfo() = clientp->svcinfo();

                clientp->disconnect();
                tls_clientp->connect(t_port);

                // SSL/TLS connection established
                if (tls_clientp->is_connected())
                {
                    tls_clientp = process_data(std::move(tls_clientp), success);
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
