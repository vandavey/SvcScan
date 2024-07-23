/*
* @file
*     hostname.cpp
* @brief
*     Source file for a network hostname.
*/
#include "includes/inet/net.h"
#include "includes/inet/net_aliases.h"
#include "includes/inet/sockets/hostname.h"

/**
* @brief
*     Initialize the object.
*/
scan::Hostname::Hostname(const string& t_name)
{
    name(t_name);
}

/**
* @brief
*     Assignment operator overload.
*/
scan::Hostname& scan::Hostname::operator=(const string& t_name)
{
    name(t_name);
    return *this;
}

/**
* @brief
*     Resolve the underlying hostname (or IPv4 address).
*/
bool scan::Hostname::resolve(const uint_t& t_retries)
{
    // Skip previously resolved name
    if (m_addr.empty())
    {
        io_context ioc;
        const results_t results{net::resolve(ioc, m_name, m_ecode, t_retries)};

        if (net::no_error(m_ecode))
        {
            m_addr = net::ipv4_from_results(results);
        }
    }
    return net::no_error(m_ecode);
}

/**
* @brief
*     Update the underlying hostname information.
*/
const std::string& scan::Hostname::name(const string& t_name)
{
    if (m_name != t_name)
    {
        reset();
        m_name = t_name;
        resolve();
    }
    return m_name;
}
