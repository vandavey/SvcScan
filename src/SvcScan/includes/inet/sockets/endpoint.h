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
#include "../../utils/algo.h"
#include "../../utils/aliases.h"
#include "../net_aliases.h"
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
        using endpoint_t = ip::tcp::endpoint;

    public:  /* Fields */
        port_t port;  // Port number
        string addr;  // Hostname or IP address

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Endpoint() noexcept : Endpoint{IPV4_ANY, PORT_NULL}
        {
        }

        constexpr Endpoint(const Endpoint&) = default;
        constexpr Endpoint(Endpoint&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Endpoint(const string& t_addr, port_t t_port = PORT_NULL) noexcept
        {
            addr = t_addr;
            port = t_port;
        }

        Endpoint(const endpoint_t& t_tcp_ep);

        virtual constexpr ~Endpoint() = default;

    public:  /* Operators */
        constexpr Endpoint& operator=(const Endpoint&) = default;
        constexpr Endpoint& operator=(Endpoint&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const override
        {
            return str();
        }

        operator endpoint_t() const;

        friend ostream& operator<<(ostream& t_os, const Endpoint& t_ep);

    public:  /* Methods */
        /**
        * @brief
        *     Get the underlying endpoint information as a string.
        */
        constexpr string str() const
        {
            return algo::fstr("%:%", addr, port);
        }
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream& operator<<(ostream& t_os, const Endpoint& t_ep)
    {
        return t_os << t_ep.str();
    }
}

#endif // !SCAN_ENDPOINT_H
