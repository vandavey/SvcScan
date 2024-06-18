/*
* @file
*     net_util.cpp
* @brief
*     Source file for network and socket utilities.
*/
#include <array>
#include <string>
#include <winerror.h>
#include <winsock2.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#include <boost/asio/error.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/beast/core/error.hpp>
#include <openssl/x509.h>
#include "includes/errors/arg_ex.h"
#include "includes/inet/net_util.h"
#include "includes/io/std_util.h"
#include "includes/utils/algorithm.h"

/**
* @brief
*     Update the given network service information
*     using the specified embedded text file resource.
*/
void scan::NetUtil::update_svc(const TextRc &t_csv_rc,
                               SvcInfo &t_info,
                               const HostState &t_state)
{
    if (!valid_port(t_info.port(), true))
    {
        throw ArgEx{ "t_info.port", "Invalid port number" };
    }
    t_info.state(t_state);

    const bool skip_info{ !t_info.summary.empty() && t_info.service == "unknown" };

    // Only resolve unknowns services
    if (t_info.service.empty() || skip_info)
    {
        if (!valid_port(t_info.port()))
        {
            throw ArgEx{ "t_info.port", "Port number must be between 0 and 65535" };
        }
        string csv_line;

        if (t_csv_rc.get_line(csv_line, static_cast<size_t>(t_info.port())))
        {
            const string_array<4> fields{ parse_fields(csv_line) };

            t_info.proto = fields[1];
            t_info.service = fields[2];

            // Update service summary
            if (!skip_info)
            {
                t_info.summary = fields[3];
            }
        }
    }
}

/**
* @brief
*     Determine whether the given socket error code is not an error.
*/
bool scan::NetUtil::no_error(const error_code &t_ecode) noexcept
{
    return t_ecode.value() == NO_ERROR;
}

/**
* @brief
*     Determine whether the given IPv4 connection endpoint is valid.
*/
bool scan::NetUtil::valid_endpoint(const Endpoint &t_ep)
{
    bool is_valid{ valid_port(t_ep.port) };

    // Only validate addresses, name resolution occurs later
    if (is_valid && valid_ipv4_fmt(t_ep.addr))
    {
        is_valid = valid_ipv4(t_ep.addr);
    }
    return is_valid;
}

/**
* @brief
*     Determine whether the given IPv4 address (dotted-quad notation) is valid.
*/
bool scan::NetUtil::valid_ipv4(const string &t_addr)
{
    bool is_valid{ false };

    if (valid_ipv4_fmt(t_addr))
    {
        int iaddr{ SOCKET_ERROR };
        const int rcode{ inet_pton(AF_INET, &t_addr[0], &iaddr) };

        is_valid = rcode == SOCKET_READY;
    }
    return is_valid;
}

/**
* @brief
*     Determine whether the given IPv4 address string
*     (dotted-quad notation) is formatted correctly.
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
* @brief
*     Determine whether the given string is a valid network port number.
*/
bool scan::NetUtil::valid_port(const string &t_port, const bool &t_ign_zero)
{
    const bool is_empty{ t_port.empty() };
    const bool is_integral{ algo::is_integral(t_port, true) };

    return !is_empty && is_integral && valid_port(std::stoi(t_port), t_ign_zero);
}

/**
* @brief
*     Write a socket error message to the standard error stream.
*/
std::string scan::NetUtil::error(const Endpoint &t_ep, const error_code &t_ecode)
{
    string msg;

    // Handle TLS errors separately
    if (t_ecode.category() == ssl_error::get_stream_category())
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
* @brief
*     Get an IPv4 address from the first result in the given DNS lookup results.
*/
std::string scan::NetUtil::ipv4_from_results(const results_t &t_results)
{
    string addr;

    if (!t_results.empty())
    {
        addr = Endpoint(*t_results.begin()).addr;
    }
    return addr;
}

/**
* @brief
*     Get the issuer name from the given X.509 certificate pointer.
*/
std::string scan::NetUtil::x509_issuer(const X509 *t_certp)
{
    string issuer;

    if (t_certp != nullptr)
    {
        issuer = x509_name(X509_get_issuer_name(t_certp));
    }
    return issuer;
}

/**
* @brief
*     Get the subject name from the given X.509 certificate pointer.
*/
std::string scan::NetUtil::x509_subject(const X509 *t_certp)
{
    string subject;

    if (t_certp != nullptr)
    {
        subject = x509_name(X509_get_subject_name(t_certp));
    }
    return subject;
}

/**
* @brief
*     Resolve the IPv4 address associated with the given TCP IPv4 endpoint.
*/
scan::results_t scan::NetUtil::resolve(io_context &t_ioc,
                                       const Endpoint &t_ep,
                                       error_code &t_ecode,
                                       const uint_t &t_retries)
{
    results_t results;
    resolver_t resolver{ t_ioc };

    // Attempt resolution for the given number of retries
    for (uint_t i{ 0U }; i <= t_retries; i++)
    {
        results = resolver.resolve(tcp::v4(),
                                   t_ep.addr,
                                   algo::to_string(t_ep.port),
                                   t_ecode);
        if (no_error(t_ecode))
        {
            break;
        }
    }
    return results;
}

/**
* @brief
*     Create an error message that corresponds to the given socket error.
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
            msg = algo::fstr("Connection refused: %/%", t_ep.port, PROTO);
            break;
        case error::connection_reset:
            msg = algo::fstr("Connection was reset: %/%", t_ep.port, PROTO);
            break;
        case error::would_block:
            msg = algo::fstr("Socket would block: %/%", t_ep.port, PROTO);
            break;
        case error::timed_out:
        case error::host_not_found_try_again:
        case static_cast<int>(beast_error::timeout):
            msg = algo::fstr("Connection timeout: %/%", t_ep.port, PROTO);
            break;
        default:
            msg = algo::fstr("%: '%'", t_ecode.value(), t_ecode.message());
            break;
    }
    return msg;
}

/**
* @brief
*     Create an error message that corresponds to the given TLS socket error.
*/
std::string scan::NetUtil::tls_error_msg(const Endpoint &t_ep, const error_code &t_ecode)
{
    string msg;

    if (t_ecode == ssl_error::stream_truncated)
    {
        msg = algo::fstr("The TLS stream was closed: %/%", t_ep.port, PROTO);
    }
    else  // Unexpected result or unspecified error
    {
        msg = algo::fstr("An unknown TLS error occurred: %/%", t_ep.port, PROTO);
    }
    return msg;
}

/**
* @brief
*     Format the given X.509 certificate name as a string.
*/
std::string scan::NetUtil::x509_name(X509_NAME *t_namep)
{
    string name;

    if (t_namep != nullptr)
    {
        name = X509_NAME_oneline(t_namep, nullptr, 0);

        if (!name.empty())
        {
            name = algo::replace(name.substr(1U), "/", ", ");
        }
    }
    return name;
}

/**
* @brief
*     Parse the string fields from the given CSV record line.
*/
scan::string_array<4> scan::NetUtil::parse_fields(const string &t_csv_line)
{
    return algo::split<4>(algo::erase(t_csv_line, "\""), ",");
}
