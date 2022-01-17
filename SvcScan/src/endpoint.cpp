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
scan::EndPoint::EndPoint()
{
    port = 0;
}

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
scan::EndPoint::EndPoint(const string &t_addr, const uint &t_port)
{
    addr = t_addr;
    port = t_port;
}

/// ***
/// Cast operator overload
/// ***
scan::EndPoint::operator string() const
{
    return str();
}

/// ***
/// Format the endpoint as a string
/// ***
std::string scan::EndPoint::str() const
{
    return Util::fstr("%:%", addr, port);
}
