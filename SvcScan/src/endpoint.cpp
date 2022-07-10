/*
*  endpoint.cpp
*  ------------
*  Source file for an IPv4 connection endpoint
*/
#include "includes/except/runtime_ex.h"
#include "includes/inet/net_util.h"

/**
* @brief  Initialize the object.
*/
scan::Endpoint::Endpoint()
{
    addr = IPV4_ANY;
    port = 0U;
}

/**
* @brief  Initialize the object.
*/
scan::Endpoint::Endpoint(const Endpoint &t_ep)
{
    *this = t_ep;
}

/**
* @brief  Initialize the object.
*/
scan::Endpoint::Endpoint(const string &t_addr, const uint &t_port)
{
    addr = t_addr;
    port = t_port;
}

/**
* @brief  Initialize the object.
*/
scan::Endpoint::Endpoint(const endpoint_t &t_tcp_ep)
{
    addr = t_tcp_ep.address().to_string();
    port = t_tcp_ep.port();
}

/**
* @brief  Copy assignment operator overload.
*/
scan::Endpoint &scan::Endpoint::operator=(const Endpoint &t_ep) noexcept
{
    addr = t_ep.addr;
    port = t_ep.port;

    return *this;
}

/**
* @brief  Cast operator overload.
*/
scan::Endpoint::operator string() const
{
    return str();
}

/**
* @brief  Cast operator overload.
*/
scan::Endpoint::operator endpoint_t() const
{
    if (NetUtil::valid_ipv4_fmt(addr) && !NetUtil::valid_ipv4(addr))
    {
        throw RuntimeEx{ "Endpoint::operator endpoint_t", "Invalid IPv4 address" };
    }
    return endpoint_t(ip::make_address_v4(addr), static_cast<port_t>(port));
}

/**
* @brief  Get the underlying endpoint information as a string.
*/
std::string scan::Endpoint::str() const
{
    return Util::fstr("%:%", addr, port);
}
