/*
*  endpoint.cpp
*  ------------
*  Source file for IPv4 connection endpoint
*/
#include "includes/net/endpoint.h"

namespace Scan
{
    using std::string;
}

/// ***
/// Initialize the object
/// ***
Scan::EndPoint::EndPoint()
{
    this->swap(m_addr, m_port);
}

/// ***
/// Initialize the object
/// ***
Scan::EndPoint::EndPoint(const EndPoint &ep)
{
    this->swap(ep.m_addr, ep.m_port);
}

/// ***
/// Initialize the object
/// ***
Scan::EndPoint::EndPoint(const string &addr, const string &port)
{
    this->swap(addr, port);
}

/// ***
/// Format the endpoint as a string
/// ***
const std::string Scan::EndPoint::str() const
{
    return (m_addr + ":" + m_port);
}

/// ***
/// Format the endpoint as a string
/// ***
std::string &Scan::EndPoint::str()
{
    return update();
}

/// ***
/// Update the string property and return as reference
/// ***
std::string &Scan::EndPoint::update()
{
    m_fmt = m_addr + ":" + m_port;
    return m_fmt;
}

/// ***
/// Swap mutable member values with reference object values
/// ***
Scan::EndPoint &Scan::EndPoint::swap(const string &addr, const string &port)
{
    m_addr = addr;
    m_port = port;

    this->addr = &m_addr;
    this->port = &m_port;

    update();
    return *this;
}
