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
scan::Args::Args()
{
    tls_enabled = false;
    verbose = false;

    threads = std::thread::hardware_concurrency();

    timeout = TcpClient::CONN_TIMEOUT;
    uri = Request<>::URI_ROOT;
}

/**
* @brief  Initialize the object.
*/
scan::Args::Args(const Args &t_args)
{
    *this = t_args;
}

/**
* @brief  Copy assignment operator overload.
*/
scan::Args &scan::Args::operator=(const Args &t_args)
{
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
