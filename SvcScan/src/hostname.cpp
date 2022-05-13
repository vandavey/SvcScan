/*
*  hostname.cpp
*  ------------
*  Source file for a network hostname
*/
#include "includes/inet/sockets/hostname.h"

/// ***
/// Initialize the object
/// ***
scan::Hostname::Hostname(const Hostname &t_hostname)
{
    m_addr = t_hostname.m_addr;
    m_ecode = t_hostname.m_ecode;
    m_name = t_hostname.m_name;
}

/// ***
/// Initialize the object
/// ***
scan::Hostname::Hostname(const string &t_name)
{
    name(t_name);
}

/// ***
/// Assignment operator overload
/// ***
scan::Hostname &scan::Hostname::operator=(const string &t_name)
{
    name(t_name);
    return *this;
}

/// ***
/// Cast operator overload
/// ***
scan::Hostname::operator string() const noexcept
{
    return m_addr.empty() ? m_name : m_addr;
}

/// ***
/// Reset the underlying hostname information
/// ***
void scan::Hostname::reset()
{
    m_ecode.clear();

    Util::clear(m_addr);
    Util::clear(m_name);
}

/// ***
/// Determine whether the underlying name van be resolved as an IPv4 address
/// ***
bool scan::Hostname::is_valid() const noexcept
{
    return !m_name.empty() && !m_addr.empty();
}

/// ***
/// Determine whether the underlying name is an IPv4 address
/// ***
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

/// ***
/// Retrieve the last DNS name resolution error code
/// ***
boost::system::error_code scan::Hostname::last_error() const noexcept
{
    return m_ecode;
}

/// ***
/// Get a constant reference to the underlying resolved address
/// ***
const std::string &scan::Hostname::addr() const noexcept
{
    return m_addr;
}

/// ***
/// Get a constant reference to the underlying name
/// ***
const std::string &scan::Hostname::name() const noexcept
{
    return m_name;
}

/// ***
/// Update the underlying hostname information
/// ***
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
