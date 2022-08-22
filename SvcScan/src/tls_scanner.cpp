/*
*  tls_scanner.cpp
*  ---------------
*  Source file for an IPv4 network scanner with SSL/TLS capabilities
*/
#include "includes/except/null_ptr_ex.h"
#include "includes/inet/scanners/tls_scanner.h"
#include "includes/resources/resource.h"

/**
* @brief  Initialize the object.
*/
scan::TlsScanner::TlsScanner(TlsScanner &&t_scanner) noexcept
    : base_t(t_scanner.m_ioc, t_scanner.m_args_ap.load()) {

    *this = std::forward<this_t>(t_scanner);
}

/**
* @brief  Initialize the object.
*/
scan::TlsScanner::TlsScanner(io_context &t_ioc, shared_ptr<Args> t_argsp)
    : base_t(t_ioc, t_argsp) {
}

/**
* @brief  Move assignment operator overload.
*/
scan::TlsScanner &scan::TlsScanner::operator=(TlsScanner &&t_scanner) noexcept
{
    if (this != &t_scanner)
    {
        m_concurrency = t_scanner.m_concurrency;
        m_conn_timeout = t_scanner.m_conn_timeout;
        m_http_uri = t_scanner.m_http_uri;
        m_services = t_scanner.m_services;
        m_status_map = t_scanner.m_status_map;
        m_timer = t_scanner.m_timer;

        m_args_ap.store(std::move(t_scanner.m_args_ap));
        m_trc_ap.store(std::move(t_scanner.m_trc_ap));

        out_path = t_scanner.out_path;
        ports = t_scanner.ports;
        target = t_scanner.target;
        verbose = t_scanner.verbose;
    }
    return *this;
}

/**
* @brief  Create a new port scan task and submit it to the underlying
*         thread pool for execution.
*/
void scan::TlsScanner::post_port_scan(const uint &t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number specified" };
    }

    // The app should have already exited
    if (!target.is_valid())
    {
        throw RuntimeEx{ "Scanner::post_task", "Invalid underlying target" };
    }

    // Post a new scan task to the thread pool
    m_pool.post([&, this]() mutable -> void
    {
        show_progress();
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

        update_status(t_port, TaskStatus::complete);
        clientp->close();
    });
}
