#include "../include/endpoint.h"

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
Scan::EndPoint::EndPoint(const std::string &addr, const std::string &port)
{
    this->swap(addr, port);
}

/// ***
/// Format the endpoint as a string
/// ***
const std::string Scan::EndPoint::str() const
{
    return (std::string(m_addr) + ":" + m_port);
}

/// ***
/// Swap mutable member values with reference object values
/// ***
Scan::EndPoint &Scan::EndPoint::swap(const std::string &addr,
                                     const std::string &port) {
    this->m_addr = addr;
    this->m_port = port;

    this->addr = &m_addr;
    this->port = &m_port;

    return *this;
}
