/*
*  endpoint.h
*  ----------
*  Header file for an IPv4 connection endpoint
*/
#pragma once

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <string>

namespace scan
{
    /// ***
    /// IPv4 TCP connection endpoint
    /// ***
    class Endpoint
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using string = std::string;

    public:  /* Fields */
        uint port;    // Port number
        string addr;  // Hostname or IP address

    public:  /* Constructors & Destructor */
        Endpoint();
        Endpoint(const Endpoint &t_ep);
        Endpoint(const string &t_addr, const uint &t_port = 0);

        virtual ~Endpoint() = default;

    public:  /* Operators */
        operator string() const;

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
