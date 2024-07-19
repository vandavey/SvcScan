/*
* @file
*     endpoint.cpp
* @brief
*     Source file for an IPv4 network endpoint.
*/
#include <sdkddkver.h>
#include <boost/asio/ip/address_v4.hpp>
#include "includes/errors/runtime_ex.h"
#include "includes/inet/net.h"
#include "includes/inet/sockets/endpoint.h"

/**
* @brief
*     Initialize the object.
*/
scan::Endpoint::Endpoint(const endpoint_t& t_tcp_ep)
{
    addr = t_tcp_ep.address().to_string();
    port = static_cast<port_t>(t_tcp_ep.port());
}

/**
* @brief
*     Cast operator overload.
*/
scan::Endpoint::operator endpoint_t() const
{
    if (net::valid_ipv4_fmt(addr) && !net::valid_ipv4(addr))
    {
        throw RuntimeEx{ "Endpoint::operator endpoint_t", "Invalid IPv4 address" };
    }
    return endpoint_t(ip::make_address_v4(addr), static_cast<port_t>(port));
}
