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

namespace scan
{
    /**
    * @brief  Command-line arguments.
    */
    class Args
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using string = std::string;

    public:  /* Fields */
        bool verbose;      // Verbose output

        string out_path;   // Output file path
        string uri;        // HTTP request URI

        Timeout timeout;   // Connection timeout
        Hostname target;   // Target hostname

        List<uint> ports;  // Target ports

    public:  /* Constructors & Destructor */
        Args();
        Args(const Args &t_args);
        Args(Args &&) = default;

        virtual ~Args() = default;

    public:  /* Operators */
        Args &operator=(const Args &t_args);
        Args &operator=(Args &&) = default;
    };
}

#endif // !ARGS_H
