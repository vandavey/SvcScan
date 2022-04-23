/*
*  timer.h
*  -------
*  Header file for timer using steady clock
*/
#pragma once

#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <sstream>
#include <string>

namespace
{
    namespace chrono = std::chrono;
}

namespace scan
{
    /// ***
    /// Scan timer with underlying steady clock
    /// ***
    class Timer
    {
    private:  /* Type Aliases */
        using milliseconds = chrono::milliseconds;
        using steady_clock = chrono::steady_clock;
        using system_clock = chrono::system_clock;
        using steady_tp    = chrono::steady_clock::time_point;
        using string       = std::string;
        using system_tp    = chrono::system_clock::time_point;

    private:  /* Constants */
        static constexpr char DT_FMT[] = "%F %T %Z";  // Date/time format

    private:  /* Fields */
        steady_tp m_steady_beg;  // Beginning steady time point
        steady_tp m_steady_end;  // Ending steady time point

        system_tp m_system_beg;  // Beginning system time point
        system_tp m_system_end;  // Ending system time point

    public:  /* Constructors & Destructor */
        Timer();
        Timer(const Timer &t_timer);
        Timer(const bool &t_start);

        virtual ~Timer() = default;

    public:  /* Methods */
        static steady_tp steady_now() noexcept;
        static system_tp system_now() noexcept;

        static string timestamp(const system_tp &t_tp,
                                const string &t_dt_fmt = DT_FMT);

        bool is_running() const noexcept;

        system_tp start() noexcept;
        system_tp stop() noexcept;

        milliseconds elapsed() const noexcept;

        system_tp beg_time() const noexcept;
        system_tp end_time() const noexcept;

        string elapsed_str() const;
        string timestamp() const;
    };
}

#endif // !TIMER_H
