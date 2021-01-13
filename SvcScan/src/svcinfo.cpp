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
Scan::SvcInfo::SvcInfo()
{
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
/// Destroy the object
/// ***
Scan::SvcInfo::~SvcInfo()
{
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
std::string Scan::SvcInfo::upto_eol(const string &data) const
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
Scan::SvcInfo &Scan::SvcInfo::parse(string &banner)
{
    if (banner.empty())
    {
        std::cout << "[x] 'banner' cannot be empty'" << std::endl;
        return *this;
    }

    banner = upto_eol(banner);
    size_t i, count = {0}, next = {0};

    // Loop through banner segments
    while ((i = banner.find_first_not_of('-', next)) != -1)
    {
        switch (count)
        {
            case 0:   // Service name
            {
                next = banner.find('-', i);
                service = banner.substr(i, (next - i));
                break;
            }
            case 1:   // Protocol version
            {
                next = banner.find('-', i);
                proto = banner.substr(i, (next - i));
                break;
            }
            case 2:   // Service version
            {
                next = banner.find('-', i);
                version = banner.substr(i, (next - i));
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

    return *this;
}
