/*
* @file
*     timer.h
* @brief
*     Header file for a steady-clock timer.
*/
#pragma once

#ifndef SCAN_TIMER_H
#define SCAN_TIMER_H

#include <chrono>
#include "algo.h"
#include "alias.h"

namespace scan
{
    /**
    * @brief
    *     Steady-clock timer.
    */
    class Timer
    {
    private:  /* Type Aliases */
        using steady_clock      = chrono::steady_clock;
        using steady_time_point = steady_clock::time_point;
        using system_clock      = chrono::system_clock;
        using system_time_point = system_clock::time_point;

    private:  /* Constants */
        static constexpr cstr_t STAMP_FMT = "{:%F %T}";  // Timestamp format

    private:  /* Fields */
        steady_time_point m_end_time;        // End steady time point
        steady_time_point m_start_time;      // Start steady time point

        system_time_point m_sys_end_time;    // End system time point
        system_time_point m_sys_start_time;  // Start system time point

    public:  /* Constructors & Destructor */
        Timer() = default;
        Timer(const Timer &) = default;
        Timer(Timer &&) = default;

        virtual ~Timer() = default;

    public:  /* Operators */
        Timer &operator=(const Timer &) = default;
        Timer &operator=(Timer &&) = default;

    public:  /* Methods */
        /**
        * @brief
        *     Get the total elapsed duration as a string.
        */
        constexpr string elapsed() const
        {
            const chrono::hh_mm_ss hms{ elapsed_ns() };

            const nanoseconds ns_subsec{ hms.subseconds() };
            const milliseconds subsec{ chrono::duration_cast<milliseconds>(ns_subsec) };

            const string time_elapsed = algo::fstr("% hr % min %.% sec",
                                                   hms.hours().count(),
                                                   hms.minutes().count(),
                                                   hms.seconds().count(),
                                                   subsec.count());
            return time_elapsed;
        }

        static string timestamp(const system_time_point &t_time);

        string end_time() const;
        string start() noexcept;
        string start_time() const;
        string stop() noexcept;

    private:  /* Methods */
        /**
        * @brief
        *     Determine whether the underlying steady timer is currently running.
        */
        constexpr bool is_running() const noexcept
        {
            return m_start_time > m_end_time && m_end_time != steady_time_point();
        }

        /**
        * @brief
        *     Get the elapsed duration in nanoseconds.
        */
        constexpr nanoseconds elapsed_ns() const noexcept
        {
            return (is_running() ? steady_clock::now() : m_end_time) - m_start_time;
        }

        /**
        * @brief
        *     Get the timer end time as a system time point.
        */
        constexpr system_time_point end_time_point() const noexcept
        {
            return m_sys_end_time;
        }

        /**
        * @brief
        *     Get the timer start time as a system time point.
        */
        constexpr system_time_point start_time_point() const noexcept
        {
            return m_sys_start_time;
        }
    };
}

#endif // !SCAN_TIMER_H
