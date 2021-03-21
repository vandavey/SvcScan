/*
*  svcinfo.cpp
*  -----------
*  Source file for TCP network application information
*/
#include <algorithm>
#include <iostream>
#include "includes/net/socket.h"
#include "includes/net/svcinfo.h"

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
Scan::SvcInfo::SvcInfo(const EndPoint &ep, const HostState &hs)
{
    this->addr = ep.addr.get();
    this->port = ep.port.get();
    this->state = hs;
}

/// ***
/// Initialize the object
/// ***
Scan::SvcInfo::SvcInfo(const EndPoint &ep, const string &banner,
                                           const HostState &hs) {
    this->addr = ep.addr.get();
    this->port = ep.port.get();
    this->state = hs;
    this->parse(banner);
}

/// ***
/// Assignment operator overload
/// ***
Scan::SvcInfo &Scan::SvcInfo::operator=(const SvcInfo &si) noexcept
{
    return swap(si);
}

/// ***
/// Parse a TCP network application banner
/// ***
void Scan::SvcInfo::parse(const string &banner_txt)
{
    if (banner_txt.empty())
    {
        return;
    }

    // Unable to detect extended service info
    if (Util::count(banner_txt, '-') < 2)
    {
        banner = Util::indent(banner_txt, 11, true);
        return;
    }
    banner = upto_eol(banner_txt);

    const vector_s vect(Util::split(banner.get(), "-", 2));

    // Analyze banner segments
    for (size_t i = {0}; i < vect.size(); i++)
    {
        switch (i)
        {
            case 0:   // Service name
            {
                service = Util::to_lower(vect[i]);
                break;
            }
            case 1:   // Protocol version
            {
                proto = Util::to_lower(vect[i]);
                service += (string(" (") + proto.get() + ")");
                break;
            }
            case 2:   // Service version
            {
                version = Util::strip(vect[i], '_');
                break;
            }
            default:
            {
                break;
            }
        }
    }
    state = HostState::open;
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
    addr = si.addr;
    banner = si.banner;
    port = si.port;
    proto = si.proto;
    service = si.service;
    state = si.state;
    version = si.version;

    return *this;
}
