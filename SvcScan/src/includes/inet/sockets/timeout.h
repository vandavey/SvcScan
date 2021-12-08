/*
*  timeout.h
*  ---------
*  Header file for a socket timeout duration
*/
#pragma once

#ifndef TIMEOUT_H
#define TIMEOUT_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <chrono>
#include <winsock2.h>
#include "../../properties/autoprop.h"

namespace
{
    namespace chrono = std::chrono;
}

namespace scan
{
    /// ***
    /// Socket timeout duration data type
    /// ***
    class Timeout
    {
    private:  /* Types */
        using uint = unsigned int;

        using milliseconds = chrono::milliseconds;
        using microseconds = chrono::microseconds;
        using seconds      = chrono::seconds;

    private:  /* Fields */
        milliseconds m_milli;  // Total milliseconds

    public:  /* Constructors & Destructor */
        Timeout();
        Timeout(const Timeout &t_timeout);
        Timeout(const uint &t_milli);

        virtual ~Timeout() = default;

    public:  /* Operators */
        Timeout &operator=(const Timeout &t_timeout);
        Timeout &operator=(const uint &t_milli);
        Timeout &operator=(const milliseconds &t_milli);

        operator uint() const noexcept;
        operator milliseconds() const noexcept;

        operator timeval() const;
    };
}

#endif // !TIMEOUT_H
