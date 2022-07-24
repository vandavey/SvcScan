/*
*  scanner.cpp
*  -----------
*  Source file for an IPv4 network scanner with SSL/TLS capabilities
*/
#include "includes/except/null_ptr_ex.h"
#include "includes/inet/scanners/tls_scanner.h"
#include "includes/resources/resource.h"

/**
* @brief  Initialize the object.
*/
scan::TlsScanner::TlsScanner(TlsScanner &&t_scanner) noexcept
    : base_t(t_scanner.m_ioc, t_scanner.m_args) {

    *this = std::forward<TlsScanner>(t_scanner);
}

/**
* @brief  Initialize the object.
*/
scan::TlsScanner::TlsScanner(io_context &t_ioc, const Args &t_args)
    : base_t(t_ioc, t_args) {

    m_tls_clientp = std::make_unique<TlsClient>(m_ioc, t_args, &m_csv_rc);
}

/**
* @brief  Destroy the object.
*/
scan::TlsScanner::~TlsScanner()
{
    error_code discard_ecode;

    if (m_clientp->is_open())
    {
        m_clientp->socket().close(discard_ecode);
    }

    if (m_tls_clientp->is_open())
    {
        m_tls_clientp->socket().close(discard_ecode);
    }
}

/**
* @brief  Move assignment operator overload.
*/
scan::TlsScanner &scan::TlsScanner::operator=(TlsScanner &&t_scanner) noexcept
{
    if (this != &t_scanner)
    {
        m_tls_clientp = std::move(t_scanner.m_tls_clientp);

        m_args = t_scanner.m_args;
        m_clientp = std::move(t_scanner.m_clientp);
        m_conn_timeout = t_scanner.m_conn_timeout;
        m_csv_rc = std::move(t_scanner.m_csv_rc);
        m_http_uri = t_scanner.m_http_uri;
        m_services = t_scanner.m_services;
        m_timer = t_scanner.m_timer;

        out_path = t_scanner.out_path;
        ports = t_scanner.ports;
        target = t_scanner.target;
        verbose = t_scanner.verbose;
    }
    return *this;
}

/**
* @brief  Close the underlying TCP and SSL/TLS clients.
*/
void scan::TlsScanner::close()
{
    if (m_clientp->is_open() && m_clientp->is_connected())
    {
        m_clientp->disconnect();
    }
    else if (m_clientp->is_open())
    {
        m_clientp->close();
    }

    if (m_tls_clientp->is_open() && m_tls_clientp->is_connected())
    {
        m_tls_clientp->disconnect();
    }
    else if (m_tls_clientp->is_open())
    {
        m_tls_clientp->close();
    }
}

/**
* @brief  Read and process the inbound socket stream data. Returns true
*         if inbound socket data was successfully processed.
*/
bool scan::TlsScanner::process_data()
{
    return process_data(m_clientp.get());
}

/**
* @brief  Read and process the inbound socket stream data. Returns true
*         if inbound socket data was successfully processed.
*/
bool scan::TlsScanner::process_data(TcpClient *t_clientp)
{
    if (t_clientp == nullptr)
    {
        throw NullPtrEx{ "t_clientp" };
    }

    if (t_clientp != m_clientp.get() && t_clientp != m_tls_clientp.get())
    {
        throw ArgEx{ "t_clientp", "Pointer should be obtained from smart pointer" };
    }

    if (!t_clientp->is_connected())
    {
        throw LogicEx{ "TlsScanner::process_data", "TCP client must be connected" };
    }

    bool data_processed{ true };
    char buffer[TcpClient::BUFFER_SIZE]{ '\0' };

    const size_t bytes_read{ t_clientp->recv(buffer) };
    HostState state{ t_clientp->host_state() };

    if (state == HostState::open)
    {
        const string recv_data{ std::string_view(&buffer[0], bytes_read) };

        // Probe HTTP version information
        if (recv_data.empty())
        {
            probe_http(t_clientp->svcinfo(), state);

            if (!net::no_error(t_clientp->last_error()))
            {
                data_processed = false;
            }
        }
        else  // Parse TCP banner data
        {
            t_clientp->svcinfo().parse(recv_data);
        }
    }
    net::update_svc(m_csv_rc, t_clientp->svcinfo(), state);

    return data_processed;
}

/**
* @brief  Scan the specified network port. Try to use an SSL/TLS encrypted TCP
*         connection if an unencrypted TCP connection cannot be established.
*/
void scan::TlsScanner::scan_port(const uint &t_port)
{
    if (!net::valid_port(t_port))
    {
        throw ArgEx{ "t_port", "Invalid port number specified" };
    }
    SvcInfo &svc_info{ m_clientp->svcinfo() };

    m_clientp->connect({ target, t_port });

    if (m_clientp->is_connected())
    {
        // Attempt SSL/TLS socket communications
        if (!process_data())
        {
            m_tls_clientp->svcinfo() = m_clientp->svcinfo();
            svc_info = m_tls_clientp->svcinfo();

            m_clientp->auto_info_reset(false);
            m_clientp->disconnect();

            m_clientp->auto_info_reset(true);
            m_tls_clientp->connect(t_port);

            // SSL/TLS connection established
            if (m_tls_clientp->is_connected())
            {
                process_data(m_tls_clientp.get());
            }
        }
    }

    m_services.add(svc_info);
    close();
}
