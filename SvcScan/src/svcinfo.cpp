/*
*  svcinfo.cpp
*  -----------
*  Source file for TCP network application information
*/
#include <iostream>
#include "includes/net/svcinfo.h"

namespace Scan
{
    using string = std::string;
}

/// ***
/// Initialize the object
/// ***
Scan::SvcInfo::SvcInfo(const SvcInfo &si)
{
    this->swap(si);
}

/// ***
/// Initialize the object
/// ***
Scan::SvcInfo::SvcInfo(const EndPoint &ep)
{
    this->ep = ep;
}

/// ***
/// Initialize the object
/// ***
Scan::SvcInfo::SvcInfo(const EndPoint &ep, const string &banner)
{
    this->ep = ep;
    this->parse(banner);
}

/// ***
/// Assignment operator overload
/// ***
Scan::SvcInfo &Scan::SvcInfo::operator=(const string &banner)
{
    return parse(const_cast<string &>(banner));
}

/// ***
/// Assignment operator overload
/// ***
Scan::SvcInfo &Scan::SvcInfo::operator=(const SvcInfo &si)
{
    return swap(si);
}

/// ***
/// Parse a TCP network application banner
/// ***
Scan::SvcInfo &Scan::SvcInfo::parse(const string &banner)
{
    if (banner.empty())
    {
        throw ArgEx("banner", "String must not be empty");
    }

    // Unable to detect extended service info
    if (Util::count(banner, '-') < 2)
    {
        m_banner = Util::indent(banner, 11, true);
        return *this;
    }
    m_banner = upto_eol(banner);

    const vector_s vect(Util::split(m_banner, "-", 2));

    // Analyze banner segments
    for (size_t i = {0}; i < vect.size(); i++)
    {
        switch (i)
        {
            case 0:   // Service name
            {
                service = vect[i];
                break;
            }
            case 1:   // Protocol version
            {
                proto = vect[i];
                break;
            }
            case 2:   // Service version
            {
                version = vect[i];
                break;
            }
            default:  // Default case
            {
                return *this;
            }
        }
    }
    return *this;
}

/// ***
/// Read string data until EOL sequence is detected
/// ***
const std::string Scan::SvcInfo::upto_eol(const string &data) const
{
    if (data.empty())
    {
        return data;
    }
    size_t index;

    // Up to NT EOL
    if ((index = data.find(Util::CRLF)) != -1)
    {
        return data.substr(0, index);
    }

    // Up to POSIX EOL
    if ((index = data.find(Util::LF)) != -1)
    {
        return data.substr(0, index);
    }
    return data;
}

/// ***
/// Swap mutable member values with reference's values
/// ***
Scan::SvcInfo &Scan::SvcInfo::swap(const SvcInfo &si) noexcept
{
    m_banner = si.m_banner;

    ep = si.ep;
    proto = si.proto;
    service = si.service;
    version = si.version;

    return *this;
}
