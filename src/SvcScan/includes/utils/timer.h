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
#include "type_defs.h"

namespace scan
{
    /**
    * @brief
    *     Steady-clock timer.
    */
    class Timer
    {
    private:  /* Type Aliases */
        using this_t = Timer;

        using steady_clock = chrono::steady_clock;
        using system_clock = chrono::system_clock;
        using steady_tp    = steady_clock::time_point;
        using system_tp    = system_clock::time_point;

    private:  /* Constants */
        static constexpr cstr_t DT_FMT = "%F %T %Z";  // Datetime format

    private:  /* Fields */
        steady_tp m_steady_beg;  // Beginning steady time point
        steady_tp m_steady_end;  // Ending steady time point

        system_tp m_system_beg;  // Beginning system time point
        system_tp m_system_end;  // Ending system time point

    public:  /* Constructors & Destructor */
        Timer() = default;
        Timer(const Timer &t_timer) noexcept;
        Timer(Timer &&) = default;
        Timer(const bool &t_start) noexcept;

        virtual ~Timer() = default;

    public:  /* Operators */
        Timer &operator=(const Timer &) = default;
        Timer &operator=(Timer &&) = default;

    public:  /* Methods */
        static steady_tp steady_now() noexcept;
        static system_tp system_now() noexcept;

        static string timestamp(const system_tp &t_tp, const string &t_fmt = DT_FMT);

        bool is_running() const noexcept;

        system_tp start() noexcept;
        system_tp stop() noexcept;

        system_tp beg_time() const noexcept;
        system_tp end_time() const noexcept;

        milliseconds elapsed() const noexcept;

        string beg_timestamp() const;
        string elapsed_str() const;
        string end_timestamp() const;
        string timestamp() const;
    };
}

#endif // !SCAN_TIMER_H
