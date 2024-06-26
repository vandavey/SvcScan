/*
* @file
*     timeout.h
* @brief
*     Header file for a network socket timeout.
*/
#pragma once

#ifndef SCAN_TIMEOUT_H
#define SCAN_TIMEOUT_H

#include <compare>
#include <winsock2.h>
#include "../../utils/alias.h"

namespace scan
{
    /**
    * @brief
    *     Network socket timeout.
    */
    class Timeout
    {
    private:  /* Fields */
        milliseconds m_milli;  // Total milliseconds

    public:  /* Constructors & Destructor */
        Timeout() noexcept;
        Timeout(const Timeout &t_timeout) noexcept;
        Timeout(Timeout &&) = default;
        Timeout(const uint_t &t_milli) noexcept;

        virtual ~Timeout() = default;

    public:  /* Operators */
        Timeout &operator=(const Timeout &t_timeout) noexcept;
        Timeout &operator=(Timeout &&) = default;
        Timeout &operator=(const uint_t &t_milli) noexcept;
        Timeout &operator=(const milliseconds &t_milli) noexcept;

        operator scan::uint_t() const noexcept;
        operator scan::milliseconds() const noexcept;
        operator timeval() const;

        std::strong_ordering operator<=>(const Timeout &) const = default;
    };
}

#endif // !SCAN_TIMEOUT_H
