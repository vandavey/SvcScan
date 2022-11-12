/*
*  svc_info.cpp
*  ------------
*  Source file for network application service information
*/
#include "includes/errors/arg_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/inet/http/request.h"
#include "includes/inet/net_util.h"
#include "includes/inet/sockets/svc_info.h"

/**
* @brief  Hide the summary field when casting to a string.
*/
bool scan::SvcInfo::no_summary{ false };

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo() noexcept
{
    req_method = verb_t::unknown;
    resp_status = status_t::unknown;

    req_httpv = { };
    resp_httpv = { };
    proto = &PROTO[0];
    req_uri = &URI_ROOT[0];

    state(HostState::unknown);
}

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo(const SvcInfo &t_info) noexcept
{
    *this = t_info;
}

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint &t_ep, const HostState &t_state) : this_t()
{
    addr = t_ep.addr;

    set_port(t_ep.port);
    state(t_state);
}

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo(const Endpoint &t_ep,
                       const string &t_banner,
                       const HostState &t_state) : this_t() {
    addr = t_ep.addr;

    set_port(t_ep.port);
    state(t_state);
    parse(t_banner);
}

/**
* @brief  Initialize the object.
*/
scan::SvcInfo::SvcInfo(const string &t_port_str,
                       const string &t_state_str,
                       const string &t_service,
                       const string &t_summary,
                       const bool &t_header) : this_t() {

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
* @brief  Copy assignment operator overload.
*/
scan::SvcInfo &scan::SvcInfo::operator=(const SvcInfo &t_info) noexcept
{
    m_port = t_info.m_port;
    m_port_str = t_info.m_port_str;
    m_state = t_info.m_state;
    m_state_str = t_info.m_state_str;

    addr = t_info.addr;
    banner = t_info.banner;
    proto = t_info.proto;
    req_headers = t_info.req_headers;
    req_httpv = t_info.req_httpv;
    req_method = t_info.req_method;
    req_uri = t_info.req_uri;
    resp_headers = t_info.resp_headers;
    resp_httpv = t_info.resp_httpv;
    resp_status = t_info.resp_status;
    service = t_info.service;
    summary = t_info.summary;

    return *this;
}

/**
* @brief  Assignment operator overload.
*/
scan::SvcInfo &scan::SvcInfo::operator=(const str_array &t_fields)
{
    service = t_fields[1];
    summary = t_fields[3];

    port_str(t_fields[0]);
    state_str(t_fields[2]);

    return *this;
}

/**
* @brief  Cast operator overload.
*/
scan::SvcInfo::operator str_array() const
{
    return str_array{ m_port_str, service, m_state_str, summary };
}

/**
* @brief  Cast operator overload.
*/
scan::SvcInfo::operator scan::string_vector() const
{
    return string_vector{ m_port_str, service, m_state_str, summary };
}

/**
* @brief  Cast operator overload.
*/
scan::SvcInfo::operator std::string() const
{
    return algo::join(operator string_vector(), no_summary ? "    " : "   ");
}

/**
* @brief  Subscript operator overload.
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
* @brief  Subscript operator overload.
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
* @brief  Equality operator overload.
*/
bool scan::SvcInfo::operator==(const SvcInfo &t_info) const noexcept
{
    const bool eq_ports{ m_port == t_info.port() };
    const bool eq_protos{ proto == t_info.proto };
    const bool eq_services{ service == t_info.service };
    const bool eq_states{ m_state == t_info.state() };
    const bool eq_summaries{ summary == t_info.summary };

    return eq_ports && eq_protos && eq_services && eq_states && eq_summaries;
}

/**
* @brief  Inequality operator overload.
*/
bool scan::SvcInfo::operator!=(const SvcInfo &t_info) const noexcept
{
    return !operator==(t_info);
}

/**
* @brief  Parse the given network application socket banner.
*/
void scan::SvcInfo::parse(const string &t_banner)
{
    if (!t_banner.empty())
    {
        banner = algo::upto_last_eol(t_banner);
        state(HostState::open);

        if (algo::count(banner, '-') >= 2)
        {
            const string_array<3> fields{ algo::split_n<3>(banner, "-") };

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
* @brief  Reset the underlying network service information.
*/
void scan::SvcInfo::reset()
{
    *this = this_t();
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
* @brief  Determine whether the given string can be parsed as a target host state.
*/
bool scan::SvcInfo::valid_state_str(const string &t_state_str) const noexcept
{
    return List<string>({ "open", "closed", "unknown" }).contains(t_state_str);
}

/**
* @brief  Get a constant reference to the underlying target host state.
*/
const scan::HostState &scan::SvcInfo::state() const noexcept
{
    return m_state;
}

/**
* @brief  Get a reference to the underlying target host state.
*/
scan::HostState &scan::SvcInfo::state() noexcept
{
    return m_state;
}

/**
* @brief  Set the value of the underlying target host state fields.
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
* @brief  Get the value of the underlying port number.
*/
unsigned int scan::SvcInfo::port() const noexcept
{
    return m_port;
}

/**
* @brief  Set the value of the underlying port number information.
*/
unsigned int scan::SvcInfo::set_port(const uint_t &t_port)
{
    if (t_port != 0U)
    {
        m_port = t_port;
        m_port_str = algo::fstr("%/%", m_port, proto);
    }
    return m_port;
}

/**
* @brief  Get a constant reference to the underlying port number string.
*/
const std::string &scan::SvcInfo::port_str() const noexcept
{
    return m_port_str;
}

/**
* @brief  Set the value of the underlying port number information.
*/
std::string &scan::SvcInfo::port_str(const string &t_port_str)
{
    const size_t sep_pos{ t_port_str.find("/") };
    m_port = static_cast<uint_t>(std::stoi(t_port_str.substr(0, sep_pos)));

    return m_port_str = t_port_str;
}

/**
* @brief  Get the string representation of the underlying host state.
*/
const std::string &scan::SvcInfo::state_str() const noexcept
{
    return m_state_str;
}

/**
* @brief  Set the value of the underlying target host state fields.
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
* @brief  Pad the underlying fields so service tables are displayed correctly.
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
* @brief  Abbreviate the given string based on the specified string length.
*/
std::string scan::SvcInfo::abbreviate(const string &t_data,
                                      const size_t &t_len) const {
    if (t_len <= 0)
    {
        throw ArgEx{ "t_len", "Length must be greater than 0" };
    }
    const string sub{ t_data.substr(0, t_len) };

    return t_data.size() > t_len ? algo::fstr("%...", sub) : sub;
}
