/*
* @file
*     svc_info.cpp
* @brief
*     Source file for network application service information.
*/
#include "includes/errors/runtime_ex.h"
#include "includes/inet/sockets/svc_info.h"
#include "includes/utils/const_defs.h"
#include "includes/utils/util.h"

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
scan::SvcInfo::SvcInfo(const Endpoint& t_ep, const HostState& t_state) : SvcInfo{}
{
    addr = t_ep.addr;

    port(t_ep.port);
    state(t_state);
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint& t_ep,
                       const string& t_banner,
                       const HostState& t_state)
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
            const string_array<3> fields{ algo::split<3>(banner, "-") };

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
std::string scan::SvcInfo::details(const bool& t_colorize) const
{
    sstream stream;

    stream << util::header_title("Details", port_str(), t_colorize, CHAR_DASH) << LF
           << util::title("Port    ", m_port, t_colorize)                      << LF
           << util::title("Protocol", proto, t_colorize)                       << LF
           << util::title("State   ", state_str(), t_colorize)                 << LF
           << util::title("Service ", service, t_colorize)                     << LF;

    // Include service summary
    if (!summary.empty())
    {
        stream << algo::concat(util::title("Summary ", summary, t_colorize), LF);
    }

    // Include raw TCP banner
    if (!banner.empty())
    {
        stream << algo::concat(util::title("Banner  ", banner, t_colorize), LF);
    }

    // Include SSL/TLS information
    if (!cipher.empty())
    {
        stream << algo::concat(LF, tls_details(t_colorize));
    }

    // Include HTTP request/response details
    if (response.valid())
    {
        stream << algo::concat(LF, req_details(t_colorize))
               << algo::concat(LF, resp_details(t_colorize));
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying HTTP request details as a
*     string. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::req_details(const bool& t_colorize) const
{
    if (!response.valid())
    {
        throw RuntimeEx{ "SvcInfo::req_details", "Invalid underlying response" };
    }
    sstream stream;

    const string version_val{ request.httpv.num_str() };
    const string method_val{ request.method_str() };
    const string headers_val{ algo::concat(LF, request.raw_headers("    ")) };

    stream << util::title("Request Version", version_val, t_colorize)   << LF
           << util::title("Request Method ", method_val, t_colorize)    << LF
           << util::title("Request URI    ", request.uri(), t_colorize) << LF
           << util::title("Request Headers", headers_val, t_colorize)   << LF;

    // Include the message body
    if (!request.body().empty())
    {
        const string body_val{ algo::concat(LF, request.body()) };
        stream << util::title("Request Body   ", body_val, t_colorize) << LF;
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying HTTP response details as a
*     string. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::resp_details(const bool& t_colorize) const
{
    if (!response.valid())
    {
        throw RuntimeEx{ "SvcInfo::resp_details", "Invalid underlying response" };
    }
    sstream stream;

    const string indent{ "    " };
    const uint_t status_val{ response.status_code() };

    const string version_val{ response.httpv.num_str() };
    const string reason_val{ response.reason() };
    const string headers_val{ algo::concat(LF, response.raw_headers(indent)) };

    stream << util::title("Response Version", version_val, t_colorize) << LF
           << util::title("Response Status ", status_val, t_colorize)  << LF
           << util::title("Response Reason ", reason_val, t_colorize)  << LF
           << util::title("Response Headers", headers_val, t_colorize) << LF;

    // Include the message body
    if (!response.body().empty())
    {
        const string body_val{ algo::concat(LF, response.body(indent)) };
        stream << util::title("Response Body   ", body_val, t_colorize) << LF;
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying SSL/TLS details. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::tls_details(const bool& t_colorize) const
{
    sstream stream;

    stream << util::title("Cipher Suite ", cipher, t_colorize)  << LF
           << util::title("X.509 Issuer ", issuer, t_colorize)  << LF
           << util::title("X.509 Subject", subject, t_colorize) << LF;

    return stream.str();
}
