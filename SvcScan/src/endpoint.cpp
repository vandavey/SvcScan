/*
*  endpoint.cpp
*  ------------
*  Source file for IPv4 connection endpoint
*/
#include "includes/net/endpoint.h"

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
scan::EndPoint::operator const std::string() const
{
    return str(addr.get(), port.get());
}

/// ***
/// Format the endpoint as a string
/// ***
const std::string scan::EndPoint::str(const string &t_addr,
                                      const string &t_port) const {
    return (t_addr + ":" + t_port);
}
