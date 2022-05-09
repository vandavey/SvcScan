/*
*  args.cpp
*  --------
*  Source file for command line arguments
*/
#include "includes/utils/args.h"
#include "includes/inet/sockets/tcp_client.h"

/// ***
/// Initialize the object
/// ***
scan::Args::Args()
{
    timeout = TcpClient::CONN_TIMEOUT;
    uri = Request<>::URI_ROOT;
    verbose = false;
}

/// ***
/// Initialize the object
/// ***
scan::Args::Args(const Args &t_args)
{
    out_path = t_args.out_path;
    ports = t_args.ports;
    target = t_args.target;
    timeout = t_args.timeout;
    uri = t_args.uri;
    verbose = t_args.verbose;
}
