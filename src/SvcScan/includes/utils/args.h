/*
* @file
*     args.h
* @brief
*     Header file for parsed command-line arguments.
*/
#pragma once

#ifndef SCAN_ARGS_H
#define SCAN_ARGS_H

#include <string>
#include "../containers/generic/list.h"
#include "../inet/net_aliases.h"
#include "../inet/net_const_defs.h"
#include "../inet/sockets/hostname.h"
#include "../inet/sockets/timeout.h"
#include "algo.h"
#include "aliases.h"

namespace scan
{
    /**
    * @brief
    *     Parsed command-line arguments.
    */
    class Args
    {
    public:  /* Fields */
        bool curl;           // Perform an HTTP GET request
        bool out_json;       // Output scan results as JSON
        bool tls_enabled;    // Use SSL/TLS scanner
        bool verbose;        // Enable verbose output

        size_t threads;      // Thread pool thread count
        Timeout timeout;     // Socket connection timeout

        string exe_path;     // Executable file path
        string out_path;     // Output file path
        string uri;          // HTTP request URI

        Hostname target;     // Target hostname

        List<string> argv;   // Command-line argument list
        List<port_t> ports;  // Target port numbers

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Args() noexcept
        {
            curl = out_json = tls_enabled = verbose = false;
            threads = 0_st;

            timeout = CONN_TIMEOUT;
            uri = URI_ROOT;
        }

        constexpr Args(const Args &) = default;
        constexpr Args(Args &&) = default;

        virtual constexpr ~Args() = default;

    public:  /* Operators */
        constexpr Args &operator=(const Args &) = default;
        constexpr Args &operator=(Args &&) = default;

    public:  /* Methods */
        /**
        * @brief
        *     Get the underlying argument list as a string enclosed in single-quotes.
        */
        constexpr string quoted_argv() const
        {
            return algo::fstr("'%'", argv.join(" "));
        }

        /**
        * @brief
        *     Get the underlying executable file path enclosed in single-quotes.
        */
        constexpr string quoted_exe_path() const
        {
            return algo::fstr("'%'", exe_path);
        }

        /**
        * @brief
        *     Get the underlying output file path enclosed in single-quotes.
        */
        constexpr string quoted_out_path() const
        {
            return algo::fstr("'%'", out_path);
        }
    };
}

#endif // !SCAN_ARGS_H
