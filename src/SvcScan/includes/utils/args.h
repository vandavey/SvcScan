/*
*  args.h
*  ------
*  Header file for command line arguments
*/
#pragma once

#ifndef ARGS_H
#define ARGS_H

#include "../containers/generic/list.h"
#include "../inet/sockets/hostname.h"
#include "../inet/sockets/timeout.h"
#include "type_defs.h"

namespace scan
{
    /**
    * @brief  Command-line arguments.
    */
    class Args
    {
    private:  /* Type Aliases */
        using algo = Algorithm;

    public:  /* Fields */
        bool curl;           // Perform an HTTP GET request
        bool out_json;       // Output scan results as JSON
        bool tls_enabled;    // Use SSL/TLS scanner
        bool verbose;        // Enable verbose output

        uint_t threads;      // Thread pool thread count
        Timeout timeout;     // Socket connection timeout

        string exe_path;     // Executable file path
        string out_path;     // Output file path
        string uri;          // HTTP request URI

        Hostname target;     // Target hostname

        List<string> argv;   // Command-line argument list
        List<port_t> ports;  // Target port numbers

    public:  /* Constructors & Destructor */
        Args() noexcept;
        Args(const Args &t_args) noexcept;
        Args(Args &&) = default;

        virtual ~Args() = default;

    public:  /* Operators */
        Args &operator=(const Args &t_args) noexcept;
        Args &operator=(Args &&) = default;

    public:  /* Methods */
        string quoted_argv() const;
        string quoted_exe_path() const;
        string quoted_out_path() const;
    };
}

#endif // !ARGS_H
