/*
*  timeout.h
*  ---------
*  Header file for a network socket timeout
*/
#pragma once

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <chrono>
#include <winsock2.h>

namespace
{
    namespace chrono = std::chrono;
}

namespace scan
{
    /**
    * @brief  Network socket timeout.
    */
    class Timeout
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using microseconds = chrono::microseconds;
        using milliseconds = chrono::milliseconds;
        using seconds      = chrono::milliseconds;

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

        bool operator==(const Timeout &t_timeout) const noexcept;
        bool operator!=(const Timeout &t_timeout) const noexcept;
    };
}

#endif // !TIMEOUT_H
