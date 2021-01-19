/*
*  svcinfo.cpp
*  -----------
*  Source file for TCP network application information
*/
#include <iostream>
#include "includes/net/svcinfo.h"

namespace Scan
{
    using std::string;
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
    this->parse(const_cast<string &>(banner));
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
/// Parse a TCP network application banner
/// ***
Scan::SvcInfo &Scan::SvcInfo::parse(const string &banner)
{
    if (banner.empty())
    {
        std::cout << "[x] 'banner' cannot be empty'" << Util::LF;
        return *this;
    }

    // Unable to detect extended service info
    if (Util::count(banner, '-') < 2)
    {
        m_banner = Util::indent(banner, 10, true);
        return *this;
    }
    const string data(m_banner = upto_eol(banner));

    int count = {0};
    size_t i, next = {0};

    // Loop through banner segments
    while ((i = data.find_first_not_of('-', next)) != -1)
    {
        switch (count)
        {
            case 0:   // Service name
            {
                next = data.find('-', i);
                service = data.substr(i, (next - i));
                break;
            }
            case 1:   // Protocol version
            {
                next = data.find('-', i);
                proto = data.substr(i, (next - i));
                break;
            }
            case 2:   // Service version
            {
                next = data.find('-', i);
                version = data.substr(i, (next - i));
                break;
            }
            default:  // Default
            {
                return *this;
            }
        }
        count += 1;
    }
    return *this;
}

/// ***
/// Swap mutable member values with reference's values
/// ***
Scan::SvcInfo &Scan::SvcInfo::swap(const SvcInfo &si)
{
    ep = si.ep;
    proto = si.proto;
    service = si.service;
    version = si.version;

    m_banner = si.m_banner;
    return *this;
}
