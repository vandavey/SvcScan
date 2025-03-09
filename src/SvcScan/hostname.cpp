/*
* @file
*     hostname.cpp
* @brief
*     Source file for a network hostname.
*/
#include "includes/inet/net.h"
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
*     Update the underlying hostname information.
*/
void scan::Hostname::name(const string& t_name)
{
    if (m_name != t_name)
    {
        reset();
        m_name = t_name;
        resolve();
    }
}

/**
* @brief
*     Resolve the underlying hostname (or IPv4 address).
*/
bool scan::Hostname::resolve(uint_t t_retries)
{
    // Skip previously resolved name
    if (m_addr.empty())
    {
        io_context io_ctx;
        const results_t results{net::resolve(io_ctx, m_name, m_ecode, t_retries)};

        if (net::no_error(m_ecode))
        {
            m_addr = net::ipv4_from_results(results);
        }
    }
    return net::no_error(m_ecode);
}
