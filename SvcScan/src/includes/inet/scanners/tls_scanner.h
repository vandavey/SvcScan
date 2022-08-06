/*
*  scanner.h
*  ---------
*  Header file for an IPv4 network scanner with SSL/TLS capabilities
*/
#pragma once

#ifndef TLS_SCANNER_H
#define TLS_SCANNER_H

#include <sdkddkver.h>
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
        using this_t = TlsScanner;

    public:  /* Constructors & Destructor */
        TlsScanner() = delete;
        TlsScanner(const TlsScanner &) = default;
        TlsScanner(TlsScanner &&t_scanner) noexcept;
        TlsScanner(io_context &t_ioc, shared_ptr<Args> t_argsp);

        virtual ~TlsScanner() = default;

    public:  /* Operators */
        TlsScanner &operator=(const TlsScanner &) = default;
        TlsScanner &operator=(TlsScanner &&t_scanner) noexcept;

    private:  /* Methods */
        void post_port_scan(const uint &t_port) override;
    };
}

#endif // !TLS_SCANNER_H
