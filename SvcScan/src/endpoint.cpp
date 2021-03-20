/*
*  endpoint.cpp
*  ------------
*  Source file for IPv4 connection endpoint
*/
#include "includes/net/endpoint.h"

/// ***
/// Initialize the object
/// ***
Scan::EndPoint::EndPoint(const EndPoint &ep)
{
    this->addr = ep.addr;
    this->port = ep.port;
}

/// ***
/// Initialize the object
/// ***
Scan::EndPoint::EndPoint(const string &addr, const string &port)
{
    this->addr = addr;
    this->port = port;
}

/// ***
/// Cast operator overload
/// ***
Scan::EndPoint::operator const std::string() const
{
    return str(addr.get(), port.get());
}

/// ***
/// Format the endpoint as a string
/// ***
const std::string Scan::EndPoint::str(const string &addr,
                                      const string &port) const {
    return (addr + ":" + port);
}
