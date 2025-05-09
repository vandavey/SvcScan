/*
* @file
*     net.cpp
* @brief
*     Source file for network and socket utilities.
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <openssl/x509.h>
#include "includes/console/util.h"
#include "includes/errors/arg_ex.h"
#include "includes/errors/error_const_defs.h"
#include "includes/inet/net.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Update the given network service information
*     using the specified embedded text file resource.
*/
void scan::net::update_svc(const TextRc& t_csv_rc, SvcInfo& t_info, HostState t_state)
{
    if (!valid_port(t_info.port(), true))
    {
        throw ArgEx{INVALID_PORTS_MSG, "t_info"};
    }

    string csv_line;
    t_info.state(t_state);

    if (t_csv_rc.get_line(csv_line, t_info.port()))
    {
        t_info.parse_csv_line(csv_line);
    }
}

/**
* @brief
*     Determine whether the given IPv4 connection endpoint is valid.
*/
bool scan::net::valid_endpoint(const Endpoint& t_ep)
{
    bool is_valid{valid_port(t_ep.port)};

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
bool scan::net::valid_ipv4(const string& t_addr)
{
    bool is_valid{false};

    if (valid_ipv4_fmt(t_addr))
    {
        int raw_addr{SOCKET_ERROR};
        const int rcode{inet_pton(AF_INET, &t_addr[0], &raw_addr)};

        is_valid = rcode == SOCKET_READY;
    }
    return is_valid;
}

/**
* @brief
*     Determine whether the given IPv4 address string
*     (dotted-quad notation) is formatted correctly.
*/
bool scan::net::valid_ipv4_fmt(const string& t_addr)
{
    bool is_valid{false};

    if (algo::count(t_addr, '.') == 3)
    {
        for (const string& octet : algo::split(t_addr, "."))
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
bool scan::net::valid_port(const string& t_port, bool t_ign_zero)
{
    const bool is_empty{t_port.empty()};
    const bool is_integral{algo::is_integral(t_port, true)};

    return !is_empty && is_integral && valid_port(std::stoi(t_port), t_ign_zero);
}

/**
* @brief
*     Write a socket error message to the standard error stream.
*/
std::string scan::net::error(const Endpoint& t_ep, const net_error_code_t& t_ecode)
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
    util::errorf(msg);

    return msg;
}

/**
* @brief
*     Get an IPv4 address from the first result in the given DNS lookup results.
*/
std::string scan::net::ipv4_from_results(const results_t& t_results)
{
    string addr;

    if (!t_results.empty())
    {
        addr = Endpoint{*t_results.begin()}.addr;
    }
    return addr;
}

/**
* @brief
*     Get the issuer name from the given X.509 certificate pointer.
*/
std::string scan::net::x509_issuer(const X509* t_certp)
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
*     Format the given X.509 certificate name as a string.
*/
std::string scan::net::x509_name(X509_NAME* t_namep)
{
    string name;

    if (t_namep != nullptr)
    {
        name = X509_NAME_oneline(t_namep, nullptr, 0);

        if (!name.empty())
        {
            name = algo::replace(name.substr(1_sz), "/", ", ");
        }
    }
    return name;
}

/**
* @brief
*     Get the subject name from the given X.509 certificate pointer.
*/
std::string scan::net::x509_subject(const X509* t_certp)
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
scan::results_t scan::net::resolve(io_context_t& t_io_ctx,
                                   const Endpoint& t_ep,
                                   net_error_code_t& t_ecode,
                                   uint_t t_retries)
{
    results_t results;
    resolver_t resolver{t_io_ctx};

    // Attempt resolution for the given number of retries
    for (uint_t i{0U}; i <= t_retries; i++)
    {
        const string port_str{algo::to_string(t_ep.port)};
        results = resolver.resolve(ip::tcp::v4(), t_ep.addr, port_str, t_ecode);

        if (no_error(t_ecode))
        {
            break;
        }
    }
    return results;
}
