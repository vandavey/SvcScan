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

namespace scan
{
    /// ***
    /// IPv4 TCP connection endpoint
    /// ***
    class EndPoint
    {
    private:  /* Types */
        using uint = unsigned int;

        using string = std::string;

    public:  /* Fields */
        AutoProp<string> addr;  // Address property
        AutoProp<uint> port;    // Port property

    public:  /* Constructors & Destructor */
        EndPoint() = default;
        EndPoint(const EndPoint &t_ep);
        EndPoint(const string &t_addr, const uint &t_port = 0);

        virtual ~EndPoint() = default;

    public:  /* Operators */
        operator string() const;

        friend std::ostream &operator<<(std::ostream &t_os, const EndPoint &t_ep);

    public:  /* Methods */
        string str() const;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const EndPoint &t_ep)
    {
        return t_os << t_ep.str();
    }
}

#endif // !END_POINT_H
