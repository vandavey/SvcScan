/*
*  hostname.cpp
*  ------------
*  Source file for a network hostname
*/
#include "includes/inet/sockets/hostname.h"

/**
* @brief  Initialize the object.
*/
scan::Hostname::Hostname(const Hostname &t_hostname)
{
    m_addr = t_hostname.m_addr;
    m_ecode = t_hostname.m_ecode;
    m_name = t_hostname.m_name;
}

/**
* @brief  Initialize the object.
*/
scan::Hostname::Hostname(const string &t_name)
{
    name(t_name);
}

/**
* @brief  Assignment operator overload.
*/
scan::Hostname &scan::Hostname::operator=(const string &t_name)
{
    name(t_name);
    return *this;
}

/**
* @brief  Cast operator overload.
*/
scan::Hostname::operator string() const noexcept
{
    return m_addr.empty() ? m_name : m_addr;
}

/**
* @brief  Reset the underlying hostname information.
*/
void scan::Hostname::reset()
{
    m_ecode.clear();

    algo::clear(m_addr);
    algo::clear(m_name);
}

/**
* @brief  Determine whether the underlying hostname can be
*         resolved as an IPv4 address.
*/
bool scan::Hostname::is_valid() const noexcept
{
    return !m_name.empty() && !m_addr.empty();
}

/**
* @brief  Resolve the underlying hostname (or IPv4 address).
*/
bool scan::Hostname::resolve(const uint &t_retries)
{
    // Skip previously resolved name
    if (m_addr.empty())
    {
        io_context ioc;
        const results_t results{ net::resolve(ioc, m_name, m_ecode, t_retries) };

        if (net::no_error(m_ecode))
        {
            m_addr = net::ipv4_from_results(results);
        }
    }
    return net::no_error(m_ecode);
}

/**
* @brief  Get the most recent DNS name resolution socket error code.
*/
boost::system::error_code scan::Hostname::last_error() const noexcept
{
    return m_ecode;
}

/**
* @brief  Get a constant reference to the underlying resolved IPv4 address.
*/
const std::string &scan::Hostname::addr() const noexcept
{
    return m_addr;
}

/**
* @brief  Get a constant reference to the underlying hostname (or IPv4 address).
*/
const std::string &scan::Hostname::name() const noexcept
{
    return m_name.empty() ? m_addr : m_name;
}

/**
* @brief  Update the underlying hostname information.
*/
const std::string &scan::Hostname::name(const string &t_name)
{
    if (m_name != t_name)
    {
        reset();
        m_name = t_name;
        resolve();
    }
    return m_name;
}
