/*
*  endpoint.h
*  ----------
*  Header file for an IPv4 connection endpoint
*/
#pragma once

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <sdkddkver.h>
#include <boost/asio/ip/tcp.hpp>
#include "../../contracts/istringcastable.h"

namespace
{
    namespace ip = boost::asio::ip;
}

namespace scan
{
    /// ***
    /// IPv4 TCP connection endpoint
    /// ***
    class Endpoint : public IStringCastable
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using port_t = ip::port_type;
        using string = std::string;
        using tcp_ep = ip::tcp::endpoint;

    public:  /* Constants */
        static constexpr char IPV4_ANY[] = "0.0.0.0";  // Unspecified IPv4 address

    public:  /* Fields */
        uint port;    // Port number
        string addr;  // Hostname or IP address

    public:  /* Constructors & Destructor */
        Endpoint();
        Endpoint(const Endpoint &t_ep);
        Endpoint(const string &t_addr, const uint &t_port = 0);
        Endpoint(const tcp_ep &t_tcp_ep);

        virtual ~Endpoint() = default;

    public:  /* Operators */
        operator string() const override;
        operator tcp_ep() const;

        friend std::ostream &operator<<(std::ostream &t_os, const Endpoint &t_ep);

    public:  /* Methods */
        string str() const;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const Endpoint &t_ep)
    {
        return t_os << t_ep.str();
    }
}

#endif // !ENDPOINT_H
