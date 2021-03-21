/*
*  endpoint.h
*  ----------
*  Header file for IPv4 connection endpoint
*/
#pragma once

#ifndef END_POINT_H
#define END_POINT_H

#include <string>
#include "../properties/autoprop.h"

namespace Scan
{
    /// ***
    /// IPv4 TCP connection endpoint
    /// ***
    class EndPoint
    {
    private:  /* Types */
        using string = std::string;

    public:  /* Fields */
        AutoProp<string> addr;  // Address property
        AutoProp<string> port;  // Port property

    public:  /* Constructors & Destructor */
        EndPoint() = default;
        EndPoint(const EndPoint &ep);
        EndPoint(const string &addr, const string &port);

        virtual ~EndPoint() = default;

    public:  /* Operators */
        operator const std::string() const;

        friend std::ostream &operator<<(std::ostream &os, const EndPoint &ep);

    private:  /* Methods */
        const string str(const string &addr, const string &port) const;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const EndPoint &ep)
    {
        return (os << ep.addr << ":" << ep.port);
    }
}

#endif // !END_POINT_H
