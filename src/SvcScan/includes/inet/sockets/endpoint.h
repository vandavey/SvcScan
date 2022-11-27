/*
*  endpoint.h
*  ----------
*  Header file for an IPv4 connection endpoint
*/
#pragma once

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "../../contracts/i_string_castable.h"
#include "../net_defs.h"

namespace scan
{
    /**
    * @brief  IPv4 TCP network endpoint.
    */
    class Endpoint : public IStringCastable
    {
    private:  /* Type Aliases */
        using port_t     = ip::port_type;
        using endpoint_t = tcp::endpoint;

    public:  /* Fields */
        uint_t port;  // Port number
        string addr;  // Hostname or IP address

    public:  /* Constructors & Destructor */
        Endpoint() noexcept;
        Endpoint(const Endpoint &t_ep) noexcept;
        Endpoint(Endpoint &&) = default;
        Endpoint(const string &t_addr, const uint_t &t_port = 0U) noexcept;
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
    * @brief  Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const Endpoint &t_ep)
    {
        return t_os << t_ep.str();
    }
}

#endif // !ENDPOINT_H
