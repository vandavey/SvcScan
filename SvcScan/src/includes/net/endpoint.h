/*
*  endpoint.h
*  ----------
*  Header file for IPv4 connection endpoint
*/
#pragma once

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <iostream>
#include <string>
#include "../properties/property.h"

namespace Scan
{
    /// ***
    /// IPv4 TCP connection endpoint
    /// ***
    class EndPoint
    {
    public:  /* Fields */
        Property<std::string> addr;  // Address property
        Property<std::string> port;  // Port property

    private:  /* Fields */
        std::string m_addr;  // Address field
        std::string m_fmt;   // Formatted as string
        std::string m_port;  // Port field

    public:  /* Constructors & Destructor */
        EndPoint();
        explicit EndPoint(const EndPoint &ep);
        EndPoint(const std::string &addr, const std::string &port);

        virtual ~EndPoint() = default;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const EndPoint &ep);

    public:  /* Methods */
        const std::string str() const;
        std::string &str();

    private:  /* Methods */
        std::string &update();
        EndPoint &swap(const std::string &addr, const std::string &port);
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const EndPoint &ep)
    {
        return (os << ep.addr << ":" << ep.port);
    }
}

#endif // !ENDPOINT_H
