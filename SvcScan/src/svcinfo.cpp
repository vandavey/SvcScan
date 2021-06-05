/*
*  svcinfo.cpp
*  -----------
*  Source file for TCP network application information
*/
#include <iostream>
#include "includes/inet/socket.h"
#include "includes/inet/svcinfo.h"

/// ***
/// Initialize the object
/// ***
scan::SvcInfo::SvcInfo(const SvcInfo &t_si)
{
    swap(t_si);
}

/// ***
/// Initialize the object
/// ***
scan::SvcInfo::SvcInfo(const EndPoint &t_ep, const HostState &t_hs)
{
    addr = t_ep.addr;
    port = std::to_string(t_ep.port);
    state = t_hs;
}

/// ***
/// Initialize the object
/// ***
scan::SvcInfo::SvcInfo(const EndPoint &t_ep,
                       const string &t_banner,
                       const HostState &t_hs) {
    addr = t_ep.addr;
    port = std::to_string(t_ep.port);
    state = t_hs;

    parse(t_banner);
}

/// ***
/// Assignment operator overload
/// ***
scan::SvcInfo &scan::SvcInfo::operator=(const SvcInfo &t_si) noexcept
{
    return swap(t_si);
}

/// ***
/// Parse a TCP network application banner
/// ***
void scan::SvcInfo::parse(const string &t_banner)
{
    if (t_banner.empty())
    {
        return;
    }

    state = HostState::open;
    banner = upto_eol(t_banner);

    // Unable to detect extended service info
    if (Util::count(banner, '-') < 2)
    {
        service = "unknown";
        info = shrink(banner);
        return;
    }

    const vector_s vect{ Util::split(banner, "-", 2) };

    // Analyze banner segments
    for (size_t i{ 0 }; i < vect.size(); i++)
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
            case 2:   // Service information
            {
                info = Util::strip(vect[i], '_');
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

/// ***
/// Shrink given string to the specified length
/// ***
std::string scan::SvcInfo::shrink(const string &t_data, const size_t &t_len) const
{
    if (t_len <= 0)
    {
        throw ArgEx("t_len", "Length must be greater than 0");
    }
    const string sub{ t_data.substr(0, t_len) };

    // Strings ending with '...' indicate shrinkage
    return (t_data.size() > t_len) ? Util::fstr("%...", sub) : sub;
}

/// ***
/// Read string data until EOL sequence is detected
/// ***
std::string scan::SvcInfo::upto_eol(const string &t_data) const
{
    if (t_data.empty())
    {
        return t_data;
    }
    size_t idx;

    // Up to NT EOL
    if ((idx = t_data.find(Util::CRLF)) != string::npos)
    {
        return t_data.substr(0, idx);
    }

    // Up to POSIX EOL
    if ((idx = t_data.find(Util::LF)) != string::npos)
    {
        return t_data.substr(0, idx);
    }
    return t_data;
}

/// ***
/// Swap mutable member values with reference's values
/// ***
scan::SvcInfo &scan::SvcInfo::swap(const SvcInfo &t_si) noexcept
{
    addr = t_si.addr;
    banner = t_si.banner;
    info = t_si.info;
    port = t_si.port;
    proto = t_si.proto;
    service = t_si.service;
    state = t_si.state;

    return *this;
}
