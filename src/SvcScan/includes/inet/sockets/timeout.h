/*
* @file
*     timeout.h
* @brief
*     Header file for a network socket timeout.
*/
#pragma once

#ifndef SCAN_TIMEOUT_H
#define SCAN_TIMEOUT_H

#include <chrono>
#include "../../utils/aliases.h"
#include "../../utils/literals.h"

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
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Timeout() noexcept : m_milli{ 0_ms }
        {
        }

        constexpr Timeout(const Timeout&) = default;
        constexpr Timeout(Timeout&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Timeout(const uint_t& t_milli) noexcept
        {
            m_milli = milliseconds(t_milli);
        }

        virtual constexpr ~Timeout() = default;

    public:  /* Operators */
        constexpr Timeout& operator=(const Timeout&) = default;
        constexpr Timeout& operator=(Timeout&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator uint_t() const noexcept
        {
            return static_cast<uint_t>(m_milli.count());
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator milliseconds() const noexcept
        {
            return m_milli;
        }

        constexpr strong_ordering operator<=>(const Timeout&) const = default;
    };
}

#endif // !SCAN_TIMEOUT_H
