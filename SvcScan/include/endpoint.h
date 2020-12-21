#pragma once

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <string>
#include "property.h"

namespace Scan
{
    /// ***
    /// IPv4 TCP connection endpoint
    /// ***
    class EndPoint
    {
    private: /* Fields */
        std::string m_addr; // Address field
        std::string m_port; // Port field

    public: /* Properties */
        Property<std::string> addr;
        Property<std::string> port;

    public: /* Constructors & Destructor */
        EndPoint();
        EndPoint(const EndPoint &ep);
        EndPoint(const std::string &addr, const std::string &port);

        virtual ~EndPoint() = default;

    public: /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const EndPoint &ep);

    public: /* Methods */
        const std::string str() const;

    private: /* Methods */
        EndPoint &swap(const std::string &addr, const std::string &port);
    };

    /// ***
    /// Left shift friend operator definition
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const EndPoint &ep)
    {
        return (os << ep.addr << ":" << ep.port);
    }
}

#endif // !ENDPOINT_H
