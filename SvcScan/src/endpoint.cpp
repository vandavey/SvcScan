/*
*  endpoint.cpp
*  ------------
*  Source file for an IPv4 connection endpoint
*/
#include "includes/inet/sockets/endpoint.h"
#include "includes/utils/util.h"

/// ***
/// Initialize the object
/// ***
scan::Endpoint::Endpoint()
{
    port = 0;
}

/// ***
/// Initialize the object
/// ***
scan::Endpoint::Endpoint(const Endpoint &t_ep)
{
    addr = t_ep.addr;
    port = t_ep.port;
}

/// ***
/// Initialize the object
/// ***
scan::Endpoint::Endpoint(const string &t_addr, const uint &t_port)
{
    addr = t_addr;
    port = t_port;
}

/// ***
/// Cast operator overload
/// ***
scan::Endpoint::operator string() const
{
    return str();
}

/// ***
/// Format the endpoint as a string
/// ***
std::string scan::Endpoint::str() const
{
    return Util::fstr("%:%", addr, port);
}
