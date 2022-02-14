/*
*  args.cpp
*  --------
*  Source file for command line arguments
*/
#pragma once

#ifndef ARGS_H
#define ARGS_H

#include <string>
#include "../containers/generic/list.h"
#include "../inet/sockets/timeout.h"

namespace scan
{
    /// ***
    /// Command-line arguments
    /// ***
    class Args
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using list_ui = List<uint>;
        using string  = std::string;

    public:  /* Fields */
        bool verbose;     // Verbose output

        string addr;      // Target address
        string out_path;  // Output file path
        string uri;       // HTTP request URI

        Timeout timeout;  // Connection timeout

        list_ui ports;    // Target ports

    public:  /* Constructors & Destructor */
        Args();
        const Args(const Args &t_args);

        virtual ~Args() = default;

    public:  /* Operators */
        Args &operator=(const Args &t_args) noexcept;
    };
}

#endif // !ARGS_H
