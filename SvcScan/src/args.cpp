/*
*  args.cpp
*  --------
*  Source file for command line arguments
*/
#include <thread>
#include "includes/inet/sockets/tcp_client.h"
#include "includes/utils/args.h"

/**
* @brief  Initialize the object.
*/
scan::Args::Args() noexcept
{
    curl = out_json = tls_enabled = verbose = false;
    threads = thread::hardware_concurrency();

    timeout = CONN_TIMEOUT;
    uri = &URI_ROOT[0];
}

/**
* @brief  Initialize the object.
*/
scan::Args::Args(const Args &t_args) noexcept
{
    *this = t_args;
}

/**
* @brief  Copy assignment operator overload.
*/
scan::Args &scan::Args::operator=(const Args &t_args) noexcept
{
    curl = t_args.curl;
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
