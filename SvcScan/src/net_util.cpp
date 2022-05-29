/*
*  net_util.cpp
*  ------------
*  Source file for network and socket utilities
*/
#include "includes/except/null_arg_ex.h"
#include "includes/inet/net_util.h"

/// ***
/// Ensure that no socket error occurred
/// ***
bool scan::NetUtil::no_error(const error_code &t_ecode) noexcept
{
    return t_ecode.value() == NO_ERROR;
}

/// ***
/// Determine whether the IPv4 connection endpoint is valid
/// ***
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

/// ***
/// Determine whether the given IPv4 (dotted-quad notation) is valid
/// ***
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

/// ***
/// Determine whether the given IPv4 (dotted-quad notation) format is valid
/// ***
bool scan::NetUtil::valid_ipv4_fmt(const string &t_addr)
{
    bool is_valid{ false };

    if (Util::count(t_addr, '.') == 3)
    {
        for (const string &octet : Util::split(t_addr, "."))
        {
            if (!(is_valid = Util::is_integral(octet)))
            {
                break;
            }
        }
    }
    return is_valid;
}

/// ***
/// Determine whether the given integer is a valid network port
/// ***
bool scan::NetUtil::valid_port(const int &t_port, const bool &t_ign_zero)
{
    bool is_valid{ t_port >= MIN_PORT && t_port <= MAX_PORT };

    if (t_ign_zero)
    {
        is_valid = is_valid || (t_port >= 0 && t_port <= MAX_PORT);
    }
    return is_valid;
}

/// ***
/// Determine whether the given string is a valid network port
/// ***
bool scan::NetUtil::valid_port(const string &t_port, const bool &t_ign_zero)
{
    const bool is_empty{ t_port.empty() };
    const bool is_integral{ Util::is_integral(t_port) };

    return !is_empty && is_integral && valid_port(std::stoi(t_port), t_ign_zero);
}

/// ***
/// Determine whether the vector integers are valid network ports
/// ***
bool scan::NetUtil::valid_port(const vector<uint> &t_ports, const bool &t_ign_zero)
{
    return std::all_of(t_ports.cbegin(), t_ports.cend(), [&](const int &l_port)
    {
        return valid_port(l_port, t_ign_zero);
    });
}

/// ***
/// Get the remote host state based on the given socket error code
/// ***
scan::HostState scan::NetUtil::host_state(const error_code &t_ecode,
                                          const bool &t_connected) noexcept {
    HostState state{ HostState::closed };

    // Determine whether the error was a timeout
    const bool is_timeout = t_ecode == error::timed_out
                         || t_ecode == beast_error::timeout;

    if (!t_connected && is_timeout)
    {
        state = HostState::unknown;
    }
    else if (no_error(t_ecode) || (t_connected && t_ecode == error::timed_out))
    {
        state = HostState::open;
    }
    return state;
}

/// ***
/// Format and print a socket error message to the standard error stream
/// ***
std::string scan::NetUtil::error(const Endpoint &t_ep, const error_code &t_ecode)
{
    string error_msg;

    switch (t_ecode.value())
    {
        case error::host_not_found:
            error_msg = Util::fstr("Unable to resolve hostname: '%'", t_ep.addr);
            break;
        case error::connection_refused:
            error_msg = Util::fstr("Connection refused: %/tcp", t_ep.port);
            break;
        case error::connection_reset:
            error_msg = Util::fstr("Connection forcibly closed: %/tcp", t_ep.port);
            break;
        case error::would_block:
            error_msg = Util::fstr("Blocking socket would block: %/tcp", t_ep.port);
            break;
        case error::timed_out:
        case int(beast_error::timeout):
        case error::host_not_found_try_again:
            error_msg = Util::fstr("Connection timeout: %/tcp", t_ep.port);
            break;
        default:
            error_msg = Util::fstr("%: '%'", t_ecode.value(), t_ecode.message());
            break;
    }

    // Write the error to stderr
    StdUtil::error(error_msg);

    return error_msg;
}

/// ***
/// Get an IPv4 address string from the given DNS lookup results
/// ***
std::string scan::NetUtil::ipv4_from_results(const results_t &t_results)
{
    string addr;

    if (!t_results.empty())
    {
        addr = static_cast<Endpoint>(*t_results.begin()).addr;
    }
    return addr;
}

/// ***
/// Modify service information for the given service reference
/// ***
scan::SvcInfo scan::NetUtil::update_svc(const TextRc &t_csv_rc,
                                        SvcInfo &t_si,
                                        const HostState &t_hs) {
    if (!valid_port(t_si.port, true))
    {
        throw ArgEx{ "t_si.port", "Invalid port number" };
    }

    t_si.state = t_hs;
    const bool skip_info{ !t_si.summary.empty() && t_si.service == "unknown" };

    // Only resolve unknowns services
    if (t_si.service.empty() || skip_info)
    {
        // Invalid port number
        if (!valid_port(t_si.port))
        {
            throw ArgEx{ "t_si.port", "Port number must be between 0 and 65535" };
        }

        string csv_line;
        const size_t line_index{ static_cast<size_t>(std::stoi(t_si.port)) - 1 };

        // Get the line from the CSV data
        if (t_csv_rc.get_line(csv_line, line_index))
        {
            const array_s fields{ parse_fields(csv_line) };

            t_si.proto = fields[1];
            t_si.service = fields[2];

            // Update service summary
            if (!skip_info)
            {
                t_si.summary = fields[3];
            }
        }
    }
    return t_si;
}

/// ***
/// Perform DNS resolution to resolve the given IPv4 endpoint
/// ***
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

/// ***
/// Parse the string fields from the given CSV record string
/// ***
scan::NetUtil::array_s scan::NetUtil::parse_fields(const string &t_csv_line)
{
    const string new_line{ Util::replace(t_csv_line, "\"", "") };
    const vector<string> field_vect{ Util::split(new_line, ",", 3) };

    array_s fields;

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
