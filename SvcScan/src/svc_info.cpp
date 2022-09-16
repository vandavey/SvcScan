/*
*  svc_info.cpp
*  ------------
*  Source file for network application service information
*/
#include "includes/except/arg_ex.h"
#include "includes/inet/net_util.h"
#include "includes/inet/sockets/svc_info.h"

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo()
{
    proto = NetUtil::PROTOCOL;
    state = HostState::unknown;
}

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo(const SvcInfo &t_info)
{
    *this = t_info;
}

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint &t_ep, const HostState &t_state)
{
    addr = t_ep.addr;
    port = std::to_string(t_ep.port);
    state = t_state;
}

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint &t_ep,
                       const string &t_banner,
                       const HostState &t_state) {
    addr = t_ep.addr;
    port = std::to_string(t_ep.port);
    state = t_state;

    parse(t_banner);
}

/**
* @brief  Copy assignment operator overload.
*/
scan::SvcInfo &scan::SvcInfo::operator=(const SvcInfo &t_info) noexcept
{
    addr = t_info.addr;
    banner = t_info.banner;
    port = t_info.port;
    proto = t_info.proto;
    service = t_info.service;
    state = t_info.state;
    summary = t_info.summary;

    return *this;
}

/**
* @brief  Parse the given network application socket banner.
*/
void scan::SvcInfo::parse(const string &t_banner)
{
    if (t_banner.empty())
    {
        return;
    }

    state = HostState::open;
    banner = upto_last_eol(t_banner);

    // Unable to detect extended service info
    if (algo::count(banner, '-') < 2)
    {
        service = "unknown";
        summary = shrink(banner);
        return;
    }
    const vector<string> vect{ algo::split(banner, "-", 2) };

    // Analyze banner segments
    for (size_t i{ 0 }; i < vect.size(); i++)
    {
        switch (i)
        {
            case 0:   // Service name
            {
                service = algo::to_lower(vect[i]);
                break;
            }
            case 1:   // Protocol version
            {
                service += algo::fstr(" (%)", algo::to_lower(vect[i]));
                break;
            }
            case 2:   // Service summary
            {
                summary = algo::replace(vect[i], "_", " ");
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

/**
* @brief  Reset the underlying network service information.
*/
void scan::SvcInfo::reset()
{
    *this = SvcInfo();
}

/**
* @brief  Reset the underlying network service information and assign a
*         value to the underlying IPv4 address field.
*/
void scan::SvcInfo::reset(const string &t_addr)
{
    reset();
    addr = t_addr;
}

/**
* @brief  Abbreviate the given string based on the specified string length.
*/
std::string scan::SvcInfo::shrink(const string &t_data, const size_t &t_len) const
{
    if (t_len <= 0)
    {
        throw ArgEx{ "t_len", "Length must be greater than 0" };
    }
    const string sub{ t_data.substr(0, t_len) };

    // String ending with '...' indicates abbreviation
    return t_data.size() > t_len ? algo::fstr("%...", sub) : sub;
}

/**
* @brief  Read the given string data until the last EOL sequence is detected.
*/
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
