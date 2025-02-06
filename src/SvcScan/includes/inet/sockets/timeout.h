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
#include "../../concepts/concepts.h"
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
    private:  /* Type Aliases */
        using rep_t = milliseconds::rep;

    private:  /* Fields */
        milliseconds m_duration;  // Timeout duration

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Timeout() noexcept : m_duration{0_ms}
        {
        }

        constexpr Timeout(const Timeout&) = default;
        constexpr Timeout(Timeout&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Timeout(Castable<rep_t> auto t_milli) noexcept
            : Timeout{milliseconds{static_cast<rep_t>(t_milli)}}
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Timeout(Duration auto t_duration) noexcept
        {
            if constexpr (Same<decltype(t_duration), milliseconds>)
            {
                m_duration = t_duration;
            }
            else  // Cast duration to milliseconds
            {
                m_duration = chrono::duration_cast<milliseconds>(t_duration);
            }
        }

        virtual constexpr ~Timeout() = default;

    public:  /* Operators */
        constexpr Timeout& operator=(const Timeout&) = default;
        constexpr Timeout& operator=(Timeout&&) = default;

        constexpr strong_ordering operator<=>(const Timeout&) const = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator uint_t() const noexcept
        {
            return static_cast<uint_t>(m_duration.count());
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator milliseconds() const noexcept
        {
            return m_duration;
        }
    };
}

#endif // !SCAN_TIMEOUT_H
