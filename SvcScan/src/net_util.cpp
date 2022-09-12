/*
*  net_util.cpp
*  ------------
*  Source file for network and socket utilities
*/
#include <boost/beast/core/error.hpp>
#include "includes/except/null_arg_ex.h"
#include "includes/inet/net_util.h"

/**
* @brief  Determine whether the given socket error code is not an error.
*/
bool scan::NetUtil::no_error(const error_code &t_ecode) noexcept
{
    return t_ecode.value() == NO_ERROR;
}

/**
* @brief  Determine whether the given IPv4 connection endpoint is valid.
*/
bool scan::NetUtil::valid_endpoint(const Endpoint &t_ep)
{
    bool is_valid{ valid_port(t_ep.port) };

    // Only validate addresses, name resolution comes later
    if (is_valid && valid_ipv4_fmt(t_ep.addr))
    {
        is_valid = valid_ipv4(t_ep.addr);
    }
    return is_valid;
}

/**
* @brief  Determine whether the given IPv4 address (dotted-quad notation) is valid.
*/
bool scan::NetUtil::valid_ipv4(const string &t_addr)
{
    bool is_valid{ false };

    if (valid_ipv4_fmt(t_addr))
    {
        int iaddr{ SOCKET_ERROR };
        const int rcode{ inet_pton(AF_INET, t_addr.c_str(), &iaddr) };

        is_valid = rcode == SOCKET_READY;
    }
    return is_valid;
}

/**
* @brief  Determine whether the given IPv4 address (dotted-quad notation)
*          is formatted correctly.
*/
bool scan::NetUtil::valid_ipv4_fmt(const string &t_addr)
{
    bool is_valid{ false };

    if (algo::count(t_addr, '.') == 3)
    {
        for (const string &octet : algo::split(t_addr, "."))
        {
            if (!(is_valid = algo::is_integral(octet)))
            {
                break;
            }
        }
    }
    return is_valid;
}

/**
* @brief  Determine whether the given integer is a valid network port number.
*/
bool scan::NetUtil::valid_port(const int &t_port, const bool &t_ign_zero)
{
    bool is_valid{ t_port >= MIN_PORT && t_port <= MAX_PORT };

    if (t_ign_zero)
    {
        is_valid = is_valid || (t_port >= 0 && t_port <= MAX_PORT);
    }
    return is_valid;
}

/**
* @brief  Determine whether the given string is a valid network port number.
*/
bool scan::NetUtil::valid_port(const string &t_port, const bool &t_ign_zero)
{
    const bool is_empty{ t_port.empty() };
    const bool is_integral{ algo::is_integral(t_port) };

    return !is_empty && is_integral && valid_port(std::stoi(t_port), t_ign_zero);
}

/**
* @brief  Determine whether the integers in the given vector
*         are valid network port numbers.
*/
bool scan::NetUtil::valid_port(const vector<uint> &t_ports, const bool &t_ign_zero)
{
    return std::ranges::all_of(t_ports, [&t_ign_zero](const int &l_port) -> bool
    {
        return valid_port(l_port, t_ign_zero);
    });
}

/**
* @brief  Write a socket error message to the standard error stream.
*/
std::string scan::NetUtil::error(const Endpoint &t_ep, const error_code &t_ecode)
{
    string msg;

    // Handle TLS errors separately
    if (t_ecode.category() == ssl::error::get_stream_category())
    {
        msg = tls_error_msg(t_ep, t_ecode);
    }
    else  // Standard socket error
    {
        msg = error_msg(t_ep, t_ecode);
    }
    StdUtil::error(msg);

    return msg;
}

/**
* @brief  Get an IPv4 address from the first result in the given DNS lookup results.
*/
std::string scan::NetUtil::ipv4_from_results(const results_t &t_results)
{
    string addr;

    if (!t_results.empty())
    {
        addr = static_cast<Endpoint>(*t_results.begin()).addr;
    }
    return addr;
}

/**
* @brief  Update the given network service information using the
*         specified embedded text file resource.
*/
scan::SvcInfo scan::NetUtil::update_svc(const TextRc &t_csv_rc,
                                        SvcInfo &t_info,
                                        const HostState &t_state) {
    if (!valid_port(t_info.port, true))
    {
        throw ArgEx{ "t_info.port", "Invalid port number" };
    }

    t_info.state = t_state;
    const bool skip_info{ !t_info.summary.empty() && t_info.service == "unknown" };

    // Only resolve unknowns services
    if (t_info.service.empty() || skip_info)
    {
        if (!valid_port(t_info.port))
        {
            throw ArgEx{ "t_info.port", "Port number must be between 0 and 65535" };
        }

        string csv_line;
        const size_t line_index{ static_cast<size_t>(std::stoi(t_info.port)) - 1 };

        if (t_csv_rc.get_line(csv_line, line_index))
        {
            const str_array fields{ parse_fields(csv_line) };

            t_info.proto = fields[1];
            t_info.service = fields[2];

            // Update service summary
            if (!skip_info)
            {
                t_info.summary = fields[3];
            }
        }
    }
    return t_info;
}

/**
* @brief  Perform DNS resolution to resolve the IPv4 address of the given endpoint.
*/
scan::NetUtil::results_t scan::NetUtil::resolve(io_context &t_ioc,
                                                const Endpoint &t_ep,
                                                error_code &t_ecode,
                                                const uint &t_retries) {
    results_t results;
    tcp::resolver resolver{ t_ioc };

    // Attempt resolution for the given number of retries
    for (uint i{ 0U }; i <= t_retries; i++)
    {
        results = resolver.resolve(tcp::v4(),
                                   t_ep.addr,
                                   std::to_string(t_ep.port),
                                   t_ecode);
        // Name was resolved
        if (no_error(t_ecode))
        {
            break;
        }
    }
    return results;
}

/**
* @brief  Create an error message that corresponds to the given socket error.
*/
std::string scan::NetUtil::error_msg(const Endpoint &t_ep, const error_code &t_ecode)
{
    string msg;

    switch (t_ecode.value())
    {
        case error::host_not_found:
            msg = algo::fstr("Unable to resolve hostname: '%'", t_ep.addr);
            break;
        case error::connection_refused:
            msg = algo::fstr("Connection refused: %/tcp", t_ep.port);
            break;
        case error::connection_reset:
            msg = algo::fstr("Connection forcibly closed: %/tcp", t_ep.port);
            break;
        case error::would_block:
            msg = algo::fstr("Blocking socket would block: %/tcp", t_ep.port);
            break;
        case error::timed_out:
        case int(boost::beast::error::timeout):
        case error::host_not_found_try_again:
            msg = algo::fstr("Connection timeout: %/tcp", t_ep.port);
            break;
        default:
            msg = algo::fstr("%: '%'", t_ecode.value(), t_ecode.message());
            break;
    }
    return msg;
}

/**
* @brief  Create an error message that corresponds to the given TLS socket error.
*/
std::string scan::NetUtil::tls_error_msg(const Endpoint &t_ep,
                                         const error_code &t_ecode) {
    string msg;

    if (t_ecode == ssl::error::stream_truncated)
    {
        msg = algo::fstr("The TLS stream was forcibly closed: %/tcp", t_ep.port);
    }
    else  // Unexpected result or unspecified error
    {
        msg = algo::fstr("An unknown TLS error occurred: %/tcp", t_ep.port);
    }
    return msg;
}

/**
* @brief  Parse the string fields from the given CSV record line.
*/
scan::NetUtil::str_array scan::NetUtil::parse_fields(const string &t_csv_line)
{
    const string new_line{ algo::replace(t_csv_line, "\"", "") };
    const vector<string> field_vect{ algo::split(new_line, ",", 3) };

    str_array fields;

    // Copy elements into array
    for (size_t i{ 0 }; i < field_vect.size(); i++)
    {
        if (i == fields.size())
        {
            break;
        }
        fields[i] = field_vect[i];
    }
    return fields;
}
