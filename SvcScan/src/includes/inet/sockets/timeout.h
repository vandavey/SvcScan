/*
*  timeout.h
*  ---------
*  Header file for a network socket timeout
*/
#pragma once

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <chrono>
#include <compare>
#include <winsock2.h>

namespace scan
{
    namespace
    {
        namespace chrono = std::chrono;
    }

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
        Timeout(Timeout &&) = default;
        Timeout(const uint &t_milli);

        virtual ~Timeout() = default;

    public:  /* Operators */
        Timeout &operator=(const Timeout &t_timeout) noexcept;
        Timeout &operator=(Timeout &&) = default;
        Timeout &operator=(const uint &t_milli) noexcept;
        Timeout &operator=(const milliseconds &t_milli) noexcept;

        operator uint() const noexcept;
        operator milliseconds() const noexcept;

        operator timeval() const;

        std::strong_ordering operator<=>(const Timeout &) const = default;
    };
}

#endif // !TIMEOUT_H
