/*
* @file
*     endpoint.h
* @brief
*     Header file for an IPv4 network endpoint.
*/
#pragma once

#ifndef SCAN_ENDPOINT_H
#define SCAN_ENDPOINT_H

#include <string>
#include <boost/asio/ip/tcp.hpp>
#include "../../contracts/i_string_castable.h"
#include "../../utils/alias.h"
#include "../net_alias.h"
#include "../net_const_defs.h"

namespace scan
{
    /**
    * @brief
    *     IPv4 TCP network endpoint.
    */
    class Endpoint : public IStringCastable
    {
    private:  /* Type Aliases */
        using endpoint_t = tcp::endpoint;

    public:  /* Fields */
        port_t port;  // Port number
        string addr;  // Hostname or IP address

    public:  /* Constructors & Destructor */
        Endpoint() noexcept;
        Endpoint(const Endpoint &t_ep) noexcept;
        Endpoint(Endpoint &&) = default;
        Endpoint(const string &t_addr, const port_t &t_port = PORT_NULL) noexcept;
        Endpoint(const endpoint_t &t_tcp_ep);

        virtual ~Endpoint() = default;

    public:  /* Operators */
        Endpoint &operator=(const Endpoint &t_ep) noexcept;
        Endpoint &operator=(Endpoint &&) = default;

        operator std::string() const override;
        operator endpoint_t() const;

        friend ostream &operator<<(ostream &t_os, const Endpoint &t_ep);

    public:  /* Methods */
        string str() const;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const Endpoint &t_ep)
    {
        return t_os << t_ep.str();
    }
}

#endif // !SCAN_ENDPOINT_H
