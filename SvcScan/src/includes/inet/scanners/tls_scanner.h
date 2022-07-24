/*
*  scanner.h
*  ---------
*  Header file for an IPv4 network scanner with SSL/TLS capabilities
*/
#pragma once

#ifndef TLS_SCANNER_H
#define TLS_SCANNER_H

#include "scanner.h"
#include "../sockets/tls_client.h"

namespace scan
{
    /**
    * @brief  IPv4 TCP and HTTP network scanner with SSL/TLS capabilities.
    */
    class TlsScanner final : public Scanner
    {
    private:  /* Type Aliases */
        using base_t = Scanner;

    private:  /* Fields */
        unique_ptr<TlsClient> m_tls_clientp;  // SSL/TLS client smart pointer

    public:  /* Constructors & Destructor */
        TlsScanner() = delete;
        TlsScanner(const TlsScanner &) = default;
        TlsScanner(TlsScanner &&t_scanner) noexcept;
        TlsScanner(io_context &t_ioc, const Args &t_args);

        virtual ~TlsScanner();

    public:  /* Operators */
        TlsScanner &operator=(const TlsScanner &) = default;
        TlsScanner &operator=(TlsScanner &&t_scanner) noexcept;

    public:  /* Methods */
        void close() override;

    private:  /* Methods */
        bool process_data() override;
        bool process_data(TcpClient *t_clientp);
        void scan_port(const uint &t_port) override;
    };
}

#endif // !TLS_SCANNER_H
