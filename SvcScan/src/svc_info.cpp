/*
*  svc_info.cpp
*  ------------
*  Source file for TCP network application information
*/
#include "includes/except/arg_ex.h"
#include "includes/inet/sockets/svc_info.h"

/// ***
/// Initialize the object
/// ***
scan::SvcInfo::SvcInfo()
{
    state = HostState::unknown;
}

/// ***
/// Initialize the object
/// ***
scan::SvcInfo::SvcInfo(const SvcInfo &t_si)
{
    operator=(t_si);
}

/// ***
/// Initialize the object
/// ***
scan::SvcInfo::SvcInfo(const Endpoint &t_ep, const HostState &t_hs)
{
    addr = t_ep.addr;
    port = std::to_string(t_ep.port);
    state = t_hs;
}

/// ***
/// Initialize the object
/// ***
scan::SvcInfo::SvcInfo(const Endpoint &t_ep,
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
    addr = t_si.addr;
    banner = t_si.banner;
    port = t_si.port;
    proto = t_si.proto;
    service = t_si.service;
    state = t_si.state;
    summary = t_si.summary;

    return *this;
}

/// ***
/// Reset the underlying service information
/// ***
void scan::SvcInfo::reset(const string &t_addr)
{
    addr.clear();
    banner.clear();
    port.clear();
    proto.clear();
    service.clear();
    summary.clear();

    addr.shrink_to_fit();
    banner.shrink_to_fit();
    port.shrink_to_fit();
    proto.shrink_to_fit();
    service.shrink_to_fit();
    summary.shrink_to_fit();

    addr = t_addr;
    state = HostState::unknown;
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
    banner = upto_last_eol(t_banner);

    // Unable to detect extended service info
    if (Util::count(banner, '-') < 2)
    {
        service = "unknown";
        summary = shrink(banner);
        return;
    }
    const vector<string> vect{ Util::split(banner, "-", 2) };

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
                service += Util::fstr(" (%)", proto);
                break;
            }
            case 2:   // Service summary
            {
                summary = Util::replace(vect[i], "_", " ");
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
        throw ArgEx{ "t_len", "Length must be greater than 0" };
    }
    const string sub{ t_data.substr(0, t_len) };

    // Strings ending with '...' indicate shrinkage
    return (t_data.size() > t_len) ? Util::fstr("%...", sub) : sub;
}

/// ***
/// Read string data until the last EOL sequence is detected
/// ***
std::string scan::SvcInfo::upto_last_eol(const string &t_data) const
{
    if (t_data.empty())
    {
        return t_data;
    }

    string buffer;
    size_t idx{ t_data.rfind(stdu::CRLF) };

    if (idx != string::npos)
    {
        buffer = t_data.substr(0, idx);
    }
    else if ((idx = t_data.rfind(stdu::LF)) != string::npos)
    {
        buffer = t_data.substr(0, idx);
    }
    return buffer;
}
