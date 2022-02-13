/*
*  args.cpp
*  --------
*  Source file for command line arguments
*/
#include "includes/utils/args.h"

/// ***
/// Initialize the object
/// ***
scan::Args::Args()
{
    uri = "/";
    verbose = false;
}

/// ***
/// Initialize the object
/// ***
scan::Args::Args(const Args &t_args)
{
    operator=(t_args);
}

/// ***
/// Assignment operator overload
/// ***
scan::Args &scan::Args::operator=(const Args &t_args) noexcept
{
    addr = t_args.addr;
    out_path = t_args.out_path;
    ports = t_args.ports;
    timeout = t_args.timeout;
    uri = t_args.uri;
    verbose = t_args.verbose;

    return *this;
}
