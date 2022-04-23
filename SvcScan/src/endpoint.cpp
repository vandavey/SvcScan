/*
*  endpoint.cpp
*  ------------
*  Source file for an IPv4 connection endpoint
*/
#include "includes/except/runtimeex.h"
#include "includes/inet/netutil.h"

/// ***
/// Initialize the object
/// ***
scan::Endpoint::Endpoint()
{
    addr = IPV4_ANY;
    port = 0U;
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
/// Initialize the object
/// ***
scan::Endpoint::Endpoint(const tcp_ep &t_tcp_ep)
{
    addr = t_tcp_ep.address().to_string();
    port = t_tcp_ep.port();
}

/// ***
/// Cast operator overload
/// ***
scan::Endpoint::operator string() const
{
    return str();
}

/// ***
/// Cast operator overload
/// ***
scan::Endpoint::operator tcp_ep() const
{
    if (NetUtil::valid_ipv4_fmt(addr) && !NetUtil::valid_ipv4(addr))
    {
        throw RuntimeEx{ "Endpoint::operator tcp_ep()", "Invalid IP address" };
    }
    return tcp_ep(ip::make_address_v4(addr), static_cast<port_t>(port));
}

/// ***
/// Format the endpoint as a string
/// ***
std::string scan::Endpoint::str() const
{
    return Util::fstr("%:%", addr, port);
}
