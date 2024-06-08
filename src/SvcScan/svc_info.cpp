/*
* @file
*     svc_info.cpp
* @brief
*     Source file for network application service information.
*/
#include <map>
#include "includes/containers/generic/list.h"
#include "includes/errors/arg_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/inet/net_expr.h"
#include "includes/inet/sockets/svc_info.h"
#include "includes/utils/expr.h"

/**
* @brief
*     Hide the summary field when casting to a string.
*/
bool scan::SvcInfo::no_summary{ false };

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
scan::SvcInfo::SvcInfo(const SvcInfo &t_info) noexcept
{
    *this = t_info;
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint &t_ep, const HostState &t_state) : this_t()
{
    addr = t_ep.addr;

    set_port(t_ep.port);
    state(t_state);
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint &t_ep,
                       const string &t_banner,
                       const HostState &t_state)
    : this_t()
{
    addr = t_ep.addr;

    set_port(t_ep.port);
    state(t_state);
    parse(t_banner);
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcInfo::SvcInfo(const string &t_port_str,
                       const string &t_state_str,
                       const string &t_service,
                       const string &t_summary,
                       const bool &t_header)
    : this_t()
{
    m_port_str = t_port_str;
    m_state_str = t_state_str;

    service = t_service;
    summary = t_summary;

    if (!t_header)
    {
        port_str(t_port_str);
        state_str(t_state_str);
    }
}

/**
* @brief
*     Copy assignment operator overload.
*/
scan::SvcInfo &scan::SvcInfo::operator=(const SvcInfo &t_info) noexcept
{
    m_port = t_info.m_port;
    m_port_str = t_info.m_port_str;
    m_state = t_info.m_state;
    m_state_str = t_info.m_state_str;

    addr = t_info.addr;
    banner = t_info.banner;
    cipher = t_info.cipher;
    issuer = t_info.issuer;
    proto = t_info.proto;
    request = t_info.request;
    response = t_info.response;
    service = t_info.service;
    subject = t_info.subject;
    summary = t_info.summary;

    return *this;
}

/**
* @brief
*     Assignment operator overload.
*/
scan::SvcInfo &scan::SvcInfo::operator=(const str_array &t_fields) noexcept
{
    service = t_fields[1];
    summary = t_fields[3];

    port_str(t_fields[0]);
    state_str(t_fields[2]);

    return *this;
}

/**
* @brief
*     Cast operator overload.
*/
scan::SvcInfo::operator str_array() const noexcept
{
    return str_array{ m_port_str, service, m_state_str, summary };
}

/**
* @brief
*     Cast operator overload.
*/
scan::SvcInfo::operator scan::string_vector() const noexcept
{
    return string_vector{ m_port_str, service, m_state_str, summary };
}

/**
* @brief
*     Cast operator overload.
*/
scan::SvcInfo::operator std::string() const
{
    return algo::join(operator string_vector(), no_summary ? "    " : "   ");
}

/**
* @brief
*     Subscript operator overload.
*/
const std::string &scan::SvcInfo::operator[](const field_t &t_field) const
{
    const string *fieldp;

    switch (t_field)
    {
        case field_t::port:
            fieldp = &m_port_str;
            break;
        case field_t::proto:
            fieldp = &proto;
            break;
        case field_t::service:
            fieldp = &service;
            break;
        case field_t::state:
            fieldp = &m_state_str;
            break;
        case field_t::summary:
            fieldp = &summary;
            break;
        default:
            throw RuntimeEx{ "SvcInfo::operator[]", "Invalid field received" };
    }
    return *fieldp;
}

/**
* @brief
*     Subscript operator overload.
*/
std::string &scan::SvcInfo::operator[](const field_t &t_field)
{
    string *fieldp;

    switch (t_field)
    {
        case field_t::port:
            fieldp = &m_port_str;
            break;
        case field_t::proto:
            fieldp = &proto;
            break;
        case field_t::service:
            fieldp = &service;
            break;
        case field_t::state:
            fieldp = &m_state_str;
            break;
        case field_t::summary:
            fieldp = &summary;
            break;
        default:
            throw RuntimeEx{ "SvcInfo::operator[]", "Invalid field received" };
    }
    return *fieldp;
}

/**
* @brief
*     Parse the given network application socket banner.
*/
void scan::SvcInfo::parse(const string &t_banner)
{
    if (!t_banner.empty())
    {
        banner = algo::upto_last_eol(t_banner);
        state(HostState::open);

        if (algo::count(banner, '-') >= 2)
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
            summary = abbreviate(algo::upto_first_eol(banner));
        }
    }
}

/**
* @brief
*     Reset the underlying network service information.
*/
void scan::SvcInfo::reset() noexcept
{
    *this = this_t();
}

/**
* @brief
*     Reset the underlying network service information and
*     assign a value to the underlying IPv4 address field.
*/
void scan::SvcInfo::reset(const string &t_addr) noexcept
{
    reset();
    addr = t_addr;
}

/**
* @brief
*     Determine whether the given string can be parsed as a target host state.
*/
bool scan::SvcInfo::valid_state_str(const string &t_state_str) const noexcept
{
    return List<string>({ "open", "closed", "unknown" }).contains(t_state_str);
}

/**
* @brief
*     Get a constant reference to the underlying target host state.
*/
const scan::HostState &scan::SvcInfo::state() const noexcept
{
    return m_state;
}

/**
* @brief
*     Get a reference to the underlying target host state.
*/
scan::HostState &scan::SvcInfo::state() noexcept
{
    return m_state;
}

/**
* @brief
*     Set the value of the underlying target host state fields.
*/
scan::HostState &scan::SvcInfo::state(const HostState &t_state) noexcept
{
    switch (m_state = t_state)
    {
        case HostState::open:
            m_state_str = "open";
            break;
        case HostState::closed:
            m_state_str = "closed";
            break;
        default:
            m_state_str = "unknown";
            break;
    }
    return m_state;
}

/**
* @brief
*     Get the value of the underlying port number.
*/
scan::port_t scan::SvcInfo::port() const noexcept
{
    return m_port;
}

/**
* @brief
*     Set the value of the underlying port number.
*/
scan::port_t scan::SvcInfo::set_port(const port_t &t_port)
{
    if (t_port != PORT_NULL)
    {
        m_port = t_port;
        m_port_str = algo::fstr("%/%", m_port, proto);
    }
    return m_port;
}

/**
* @brief
*     Get the underlying service details as a string.
*     Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::details(const bool &t_colorize) const
{
    sstream stream;

    stream << stdu::header_title("Details", m_port_str, t_colorize, '-') << LF
           << stdu::title("Port    ", m_port, t_colorize)                << LF
           << stdu::title("Protocol", proto, t_colorize)                 << LF
           << stdu::title("State   ", m_state_str, t_colorize)           << LF
           << stdu::title("Service ", service, t_colorize)               << LF;

    // Include service summary
    if (!summary.empty())
    {
        stream << algo::concat(stdu::title("Summary ", summary, t_colorize), LF);
    }

    // Include raw TCP banner
    if (!banner.empty())
    {
        stream << algo::concat(stdu::title("Banner  ", banner, t_colorize), LF);
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
*     Get a constant reference to the underlying port number string.
*/
const std::string &scan::SvcInfo::port_str() const noexcept
{
    return m_port_str;
}

/**
* @brief
*     Set the value of the underlying port number information.
*/
std::string &scan::SvcInfo::port_str(const string &t_port_str)
{
    const size_t sep_pos{ t_port_str.find("/") };
    m_port = algo::to_uint(t_port_str.substr(0, sep_pos));

    return m_port_str = t_port_str;
}

/**
* @brief
*     Get the string representation of the underlying host state.
*/
const std::string &scan::SvcInfo::state_str() const noexcept
{
    return m_state_str;
}

/**
* @brief
*     Set the value of the underlying target host state fields.
*/
std::string &scan::SvcInfo::state_str(const string &t_state_str)
{
    if (!valid_state_str(t_state_str))
    {
        throw ArgEx{ "t_state_str", "Invalid host state string received" };
    }

    if (t_state_str == "open")
    {
        state(HostState::open);
    }
    else if (t_state_str == "closed")
    {
        state(HostState::closed);
    }
    else if (t_state_str == "unknown")
    {
        state(HostState::unknown);
    }
    return m_state_str;
}

/**
* @brief
*     Pad the underlying fields so service tables are displayed correctly.
*/
scan::SvcInfo::str_array scan::SvcInfo::pad_fields(const field_map &t_map) const
{
    SvcInfo info{ *this };

    for (const field_map::value_type &pair : t_map)
    {
        // Avoid trailing whitespace
        if (pair.first == field_t::summary)
        {
            continue;
        }
        const field_t field_name{ pair.first };

        const size_t max_width{ pair.second };
        const size_t width{ (*this)[field_name].size() };

        if (max_width < width)
        {
            throw RuntimeEx{ "SvcInfo::pad_fields", "Invalid width value in map" };
        }
        const size_t delta{ max_width - width };

        if (delta > 0)
        {
            info[field_name] = (*this)[field_name] + string(delta, ' ');
        }
    }
    return info;
}

/**
* @brief
*     Abbreviate the given string based on the specified string length.
*/
std::string scan::SvcInfo::abbreviate(const string &t_data, const size_t &t_len) const
{
    if (t_len <= 0)
    {
        throw ArgEx{ "t_len", "Length must be greater than 0" };
    }
    const string sub{ t_data.substr(0, t_len) };

    return t_data.size() > t_len ? algo::fstr("%...", sub) : sub;
}

/**
* @brief
*     Get the underlying HTTP request details as a
*     string. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::req_details(const bool &t_colorize) const
{
    if (!response.valid())
    {
        throw RuntimeEx{ "SvcInfo::req_details", "Invalid underlying response" };
    }
    sstream stream;

    const string version_val{ request.httpv.num_str() };
    const string method_val{ request.method_str() };
    const string headers_val{ algo::concat(LF, request.raw_headers("    ")) };

    stream << stdu::title("Request Version", version_val, t_colorize)   << LF
           << stdu::title("Request Method ", method_val, t_colorize)    << LF
           << stdu::title("Request URI    ", request.uri(), t_colorize) << LF
           << stdu::title("Request Headers", headers_val, t_colorize)   << LF;

    // Include the message body
    if (!request.body().empty())
    {
        const string body_val{ algo::concat(LF, request.body()) };
        stream << stdu::title("Request Body   ", body_val, t_colorize) << LF;
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying HTTP response details as a
*     string. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::resp_details(const bool &t_colorize) const
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

    stream << stdu::title("Response Version", version_val, t_colorize) << LF
           << stdu::title("Response Status ", status_val, t_colorize)  << LF
           << stdu::title("Response Reason ", reason_val, t_colorize)  << LF
           << stdu::title("Response Headers", headers_val, t_colorize) << LF;

    // Include the message body
    if (!response.body().empty())
    {
        const string body_val{ algo::concat(LF, response.body(indent)) };
        stream << stdu::title("Response Body   ", body_val, t_colorize) << LF;
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying SSL/TLS details. Optionally colorize the resulting details.
*/
std::string scan::SvcInfo::tls_details(const bool &t_colorize) const
{
    sstream stream;

    stream << stdu::title("Cipher Suite ", cipher, t_colorize)  << LF
           << stdu::title("X.509 Issuer ", issuer, t_colorize)  << LF
           << stdu::title("X.509 Subject", subject, t_colorize) << LF;

    return stream.str();
}
