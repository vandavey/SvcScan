/*
* @file
*     svc_info.cpp
* @brief
*     Source file for network application service information.
*/
#include "includes/console/util.h"
#include "includes/errors/runtime_ex.h"
#include "includes/inet/services/svc_info.h"
#include "includes/utils/const_defs.h"

/**
* @brief
*     Initialize the object.
*/
scan::SvcInfo::SvcInfo() noexcept
{
    proto = PROTO;
    state(HostState::unknown);
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint& t_ep, HostState t_state) : SvcInfo{}
{
    addr = t_ep.addr;

    port(t_ep.port);
    state(t_state);
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint& t_ep, const string& t_banner, HostState t_state)
    : SvcInfo{}
{
    addr = t_ep.addr;

    port(t_ep.port);
    state(t_state);
    parse(t_banner);
}

/**
* @brief
*     Parse the given network application socket banner.
*/
void scan::SvcInfo::parse(const string& t_banner)
{
    if (!t_banner.empty())
    {
        banner = algo::up_to_last_eol(t_banner);
        state(HostState::open);

        if (algo::count(banner, CHAR_DASH) >= 2)
        {
            const string_array<3> fields{algo::split<3>(banner, "-")};

            service = algo::fstr("% (%)",
                                 algo::to_lower(fields[0]),
                                 algo::to_lower(fields[1]));

            summary = algo::replace(fields[2], "_", " ");
        }
        else  // Unable to detect extended service info
        {
            service = "unknown";
            summary = abbreviate<35>(algo::up_to_first_eol(banner));
        }
    }
}

/**
* @brief
*     Reset the underlying network service information.
*/
void scan::SvcInfo::reset() noexcept
{
    *this = SvcInfo();
}

/**
* @brief
*     Reset the underlying network service information and
*     assign a value to the underlying IPv4 address field.
*/
void scan::SvcInfo::reset(const string& t_addr) noexcept
{
    reset();
    addr = t_addr;
}

/**
* @brief
*     Get the underlying service details as a string.
*     Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::details(bool t_colorize) const
{
    sstream stream;

    stream << util::fmt_title("Details", port_str(), t_colorize, CHAR_DASH)    << LF
           << util::fmt_field("Port    ", algo::to_string(m_port), t_colorize) << LF
           << util::fmt_field("Protocol", proto, t_colorize)                   << LF
           << util::fmt_field("State   ", state_str(), t_colorize)             << LF
           << util::fmt_field("Service ", service, t_colorize)                 << LF;

    // Include service summary
    if (!summary.empty())
    {
        stream << util::fmt_field("Summary ", summary, t_colorize) << LF;
    }

    // Include raw TCP banner
    if (!banner.empty())
    {
        stream << util::fmt_field("Banner  ", banner, t_colorize) << LF;
    }

    // Include SSL/TLS information
    if (!cipher.empty())
    {
        stream << LF << tls_details(t_colorize);
    }

    // Include HTTP request/response details
    if (response.valid())
    {
        stream << LF << request_details(t_colorize)
               << LF << response_details(t_colorize);
    }
    return algo::normalize_eol(stream.str());
}

/**
* @brief
*     Get the underlying HTTP request details as a
*     string. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::request_details(bool t_colorize) const
{
    if (!response.valid())
    {
        throw RuntimeEx{"SvcInfo::request_details", "Invalid underlying response"};
    }
    sstream stream;

    const string version{request.httpv.num_str()};
    const string method{request.method_str()};

    stream << util::fmt_field("Request Version", version, t_colorize)       << LF
           << util::fmt_field("Request Method ", method, t_colorize)        << LF
           << util::fmt_field("Request URI    ", request.uri(), t_colorize) << LF
           << util::fmt_field("Request Headers", t_colorize)                << LF
           << request.raw_headers(algo::pad(4_st))                          << LF;

    // Include the message body
    if (!request.body().empty())
    {
        stream << util::fmt_field("Request Body   ", t_colorize) << LF
               << request.body(algo::pad(4_st))                  << LF;
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying HTTP response details as a
*     string. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::response_details(bool t_colorize) const
{
    if (!response.valid())
    {
        throw RuntimeEx{"SvcInfo::response_details", "Invalid underlying response"};
    }
    sstream stream;

    const string version{response.httpv.num_str()};
    const string status{algo::to_string(response.status_code())};
    const string reason{response.reason()};

    stream << util::fmt_field("Response Version", version, t_colorize) << LF
           << util::fmt_field("Response Status ", status, t_colorize)  << LF
           << util::fmt_field("Response Reason ", reason, t_colorize)  << LF
           << util::fmt_field("Response Headers", t_colorize)          << LF
           << response.raw_headers(algo::pad(4_st))                    << LF;

    // Include the message body
    if (!response.body().empty())
    {
        stream << util::fmt_field("Response Body   ", t_colorize) << LF
               << response.body(algo::pad(4_st))                  << LF;
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying SSL/TLS details. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::tls_details(bool t_colorize) const
{
    sstream stream;

    stream << util::fmt_field("Cipher Suite ", cipher, t_colorize)  << LF
           << util::fmt_field("X.509 Issuer ", issuer, t_colorize)  << LF
           << util::fmt_field("X.509 Subject", subject, t_colorize) << LF;

    return stream.str();
}
