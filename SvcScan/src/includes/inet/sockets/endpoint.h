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
#include "../../contracts/i_string_castable.h"

namespace
{
    namespace ip = boost::asio::ip;
}

namespace scan
{
    /**
    * @brief  IPv4 TCP network endpoint.
    */
    class Endpoint : public IStringCastable
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using endpoint_t = ip::tcp::endpoint;
        using port_t     = ip::port_type;
        using string     = std::string;

    public:  /* Constants */
        static constexpr char IPV4_ANY[] = "0.0.0.0";  // Unspecified IPv4 address

    public:  /* Fields */
        uint port;    // Port number
        string addr;  // Hostname or IP address

    public:  /* Constructors & Destructor */
        Endpoint();
        Endpoint(const Endpoint &t_ep);
        Endpoint(const string &t_addr, const uint &t_port = 0);
        Endpoint(const endpoint_t &t_tcp_ep);

        virtual ~Endpoint() = default;

    public:  /* Operators */
        operator string() const override;
        operator endpoint_t() const;

        friend std::ostream &operator<<(std::ostream &t_os, const Endpoint &t_ep);

    public:  /* Methods */
        string str() const;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline std::ostream &operator<<(std::ostream &t_os, const Endpoint &t_ep)
    {
        return t_os << t_ep.str();
    }
}

#endif // !ENDPOINT_H
