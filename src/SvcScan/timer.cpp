/*
*  timer.cpp
*  ---------
*  Source file for a timer with an underlying steady clock
*/
#include <iomanip>
#include "includes/utils/algorithm.h"
#include "includes/utils/timer.h"

/**
* @brief  Initialize the object.
*/
scan::Timer::Timer()
{
    m_steady_beg = m_steady_end = steady_tp::time_point();
    m_system_beg = m_system_end = system_tp::time_point();
}

/**
* @brief  Initialize the object.
*/
scan::Timer::Timer(const Timer &t_timer)
{
    m_steady_beg = t_timer.m_steady_beg;
    m_steady_end = t_timer.m_steady_end;

    m_system_beg = t_timer.m_system_beg;
    m_system_end = t_timer.m_system_end;
}

/**
* @brief  Initialize the object.
*/
scan::Timer::Timer(const bool &t_start) : this_t()
{
    if (t_start)
    {
        start();
    }
}

/**
* @brief  Get the current system datetime as a steady time point.
*/
scan::Timer::steady_tp scan::Timer::steady_now() noexcept
{
    return steady_clock::now();
}

/**
* @brief  Get the current system datetime as a system time point.
*/
scan::Timer::system_tp scan::Timer::system_now() noexcept
{
    return system_clock::now();
}

/**
* @brief  Get a timestamp representing the given time point
*         in the specified datetime format.
*/
std::string scan::Timer::timestamp(const system_tp &t_tp, const string &t_fmt)
{
    tm tm_time{ 0 };

    const time_t tt_time{ system_clock::to_time_t(t_tp) };
    localtime_s(&tm_time, &tt_time);

    return Algorithm::to_string(std::put_time(&tm_time, &t_fmt[0]));
}

/**
* @brief  Determine whether the underlying steady timer is currently running.
*/
bool scan::Timer::is_running() const noexcept
{
    const bool is_default{ m_steady_beg == steady_tp::time_point() };
    return !is_default && (m_steady_beg == m_steady_end);
}

/**
* @brief  Start the underlying steady timer.
*/
scan::Timer::system_tp scan::Timer::start() noexcept
{
    m_steady_beg = m_steady_end = steady_now();
    return m_system_beg = m_system_end = system_now();
}

/**
* @brief  Stop the underlying steady timer.
*/
scan::Timer::system_tp scan::Timer::stop() noexcept
{
    m_steady_end = steady_now();
    return m_system_end = system_now();
}

/**
* @brief  Get the timer start time as a system time point.
*/
scan::Timer::system_tp scan::Timer::beg_time() const noexcept
{
    return m_system_beg;
}

/**
* @brief  Get the timer end time as a system time point.
*/
scan::Timer::system_tp scan::Timer::end_time() const noexcept
{
    return is_running() ? system_now() : m_system_end;
}

/**
* @brief  Calculate the elapsed duration in milliseconds.
*/
scan::milliseconds scan::Timer::elapsed() const noexcept
{
    const steady_tp steady_end{ is_running() ? steady_now() : m_steady_end };
    return chrono::duration_cast<milliseconds>(steady_end - m_steady_beg);
}

/**
* @brief  Get a timestamp representing the underlying start time point
*         in the default datetime format.
*/
std::string scan::Timer::beg_timestamp() const
{
    return timestamp(m_system_beg);
}

/**
* @brief  Get the total elapsed duration as a string.
*/
std::string scan::Timer::elapsed_str() const
{
    sstream stream;
    milliseconds ms{ elapsed() };

    // Calculate duration in hours
    if (ms >= hours(1))
    {
        const hours hr_floor{ chrono::floor<hours>(ms) };
        ms -= hr_floor;

        const string hr_noun{ (hr_floor < hours(2)) ? "hour" : "hours" };
        stream << Algorithm::fstr("% %, ", hr_floor.count(), hr_noun);
    }

    const minutes min_floor{ chrono::floor<minutes>(ms) };
    ms -= min_floor;

    const seconds sec_floor{ chrono::floor<seconds>(ms) };
    ms -= sec_floor;

    // Get the duration as a fraction of seconds
    const string sec_fraction{ std::to_string(double(ms.count()) / 1000) };

    // Interpolate the durations values
    stream << Algorithm::fstr("% min, %.% sec",
                              min_floor.count(),
                              sec_floor.count(),
                              sec_fraction.substr(2, 3));
    return stream.str();
}

/**
* @brief  Get a timestamp representing the underlying end time point
*         in the default datetime format.
*/
std::string scan::Timer::end_timestamp() const
{
    return timestamp(m_system_end);
}

/**
* @brief  Get a timestamp representing the current system datetime
*         in the default datetime format.
*/
std::string scan::Timer::timestamp() const
{
    return timestamp(system_now());
}
