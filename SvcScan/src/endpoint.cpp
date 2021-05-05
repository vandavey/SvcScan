/*
*  endpoint.cpp
*  ------------
*  Source file for IPv4 connection endpoint
*/
#include "includes/inet/endpoint.h"

/// ***
/// Initialize the object
/// ***
scan::EndPoint::EndPoint(const EndPoint &t_ep)
{
    addr = t_ep.addr;
    port = t_ep.port;
}

/// ***
/// Initialize the object
/// ***
scan::EndPoint::EndPoint(const string &t_addr, const string &t_port)
{
    addr = t_addr;
    port = t_port;
}

/// ***
/// Cast operator overload
/// ***
scan::EndPoint::operator string() const
{
    return str(addr, port);
}

/// ***
/// Format the endpoint as a string
/// ***
std::string scan::EndPoint::str(const string &t_addr,
                                const string &t_port) const {
    // Join addr and port
    return (t_addr + ":" + t_port);
}
