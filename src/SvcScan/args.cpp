/*
* @file
*     args.cpp
* @brief
*     Source file for parsed command-line arguments.
*/
#include <string>
#include <thread>
#include "includes/inet/net_expr.h"
#include "includes/threading/thread_alias.h"
#include "includes/utils/algorithm.h"
#include "includes/utils/args.h"

/**
* @brief
*     Initialize the object.
*/
scan::Args::Args() noexcept
{
    curl = out_json = tls_enabled = verbose = false;
    threads = thread::hardware_concurrency();

    timeout = CONN_TIMEOUT;
    uri = URI_ROOT;
}

/**
* @brief
*     Initialize the object.
*/
scan::Args::Args(const Args &t_args) noexcept
{
    *this = t_args;
}

/**
* @brief
*     Copy assignment operator overload.
*/
scan::Args &scan::Args::operator=(const Args &t_args) noexcept
{
    argv = t_args.argv;
    curl = t_args.curl;
    exe_path = t_args.exe_path;
    out_json = t_args.out_json;
    out_path = t_args.out_path;
    ports = t_args.ports;
    target = t_args.target;
    threads = t_args.threads;
    timeout = t_args.timeout;
    tls_enabled = t_args.tls_enabled;
    uri = t_args.uri;
    verbose = t_args.verbose;

    return *this;
}

/**
* @brief
*     Get the underlying argument list as a string enclosed in single-quotes.
*/
std::string scan::Args::quoted_argv() const
{
    return algo::fstr("'%'", argv.join(" "));
}

/**
* @brief
*     Get the underlying executable file path enclosed in single-quotes.
*/
std::string scan::Args::quoted_exe_path() const
{
    return algo::fstr("'%'", exe_path);
}

/**
* @brief
*     Get the underlying output file path enclosed in single-quotes.
*/
std::string scan::Args::quoted_out_path() const
{
    return algo::fstr("'%'", out_path);
}
