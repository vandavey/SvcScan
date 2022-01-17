/*
*  timer.cpp
*  ---------
*  Source file for timer using steady clock
*/
#include <iomanip>
#include "includes/utils/timer.h"

/// ***
/// Initialize the object
/// ***
scan::Timer::Timer()
{
    m_steady_beg = m_steady_end = steady_tp::time_point();
    m_system_beg = m_system_end = system_tp::time_point();
}

/// ***
/// Initialize the object
/// ***
scan::Timer::Timer(const Timer &t_timer)
{
    m_steady_beg = t_timer.m_steady_beg;
    m_steady_end = t_timer.m_steady_end;

    m_system_beg = t_timer.m_system_beg;
    m_system_end = t_timer.m_system_end;
}

/// ***
/// Initialize the object
/// ***
scan::Timer::Timer(const bool &t_start) : Timer()
{
    if (t_start)
    {
        start();
    }
}

/// ***
/// Utility - Retrieve the current steady time
/// ***
scan::Timer::steady_tp scan::Timer::steady_now() noexcept
{
    return steady_clock::now();
}

/// ***
/// Utility - Retrieve the current system time
/// ***
scan::Timer::system_tp scan::Timer::system_now() noexcept
{
    return system_clock::now();
}

/// ***
/// Utility - Get string representation of the given time point
/// ***
std::string scan::Timer::timestamp(const system_tp &t_tp, const string &t_dt_fmt)
{
    sstream ss;
    const time_t tt{ system_clock::to_time_t(t_tp) };

    tm time{ 0 };
    localtime_s(&time, &tt);

    ss << std::put_time(&time, t_dt_fmt.c_str());

    return ss.str();
}

/// ***
/// Determine if the timer has started and is currently running
/// ***
bool scan::Timer::is_running() const noexcept
{
    const bool is_default{ m_steady_beg == steady_tp::time_point() };
    return !is_default && (m_steady_beg == m_steady_end);
}

/// ***
/// Start the scan duration timer
/// ***
scan::Timer::system_tp scan::Timer::start() noexcept
{
    m_steady_beg = m_steady_end = steady_now();
    return m_system_beg = m_system_end = system_now();
}

/// ***
/// Stop the scan duration timer
/// ***
scan::Timer::system_tp scan::Timer::stop() noexcept
{
    m_steady_end = steady_now();
    return m_system_end = system_now();
}

/// ***
/// Calculate the elapsed duration in milliseconds
/// ***
chrono::milliseconds scan::Timer::elapsed() const noexcept
{
    const steady_tp steady_end{ is_running() ? steady_now() : m_steady_end };
    return chrono::duration_cast<milliseconds>(steady_end - m_steady_beg);
}

/// ***
/// Timer start time as system a time point
/// ***
scan::Timer::system_tp scan::Timer::beg_time() const noexcept
{
    return m_system_beg;
}

/// ***
/// Timer end time as system a time point
/// ***
scan::Timer::system_tp scan::Timer::end_time() const noexcept
{
    return is_running() ? system_now() : m_system_end;
}

/// ***
/// Get the elapsed duration length as a string
/// ***
std::string scan::Timer::elapsed_str() const
{
    sstream ss;
    milliseconds ms{ elapsed() };

    // Calculate duration in hours
    if (ms >= hours(1))
    {
        const hours hr{ chrono::floor<hours>(ms) };
        ms -= hr;

        ss << hr.count() << " ";
        (hr < hours(2)) ? (ss << "hour, ") : (ss << "hours, ");
    }

    // Calculate duration in minutes
    const minutes min{ chrono::floor<minutes>(ms) };

    ss << min.count() << " min, ";
    ms -= min;

    // Calculate duration in seconds
    const seconds sec{ chrono::floor<seconds>(ms) };

    ss << sec.count() << ".";
    ms -= sec;

    // Format milliseconds as seconds ratio
    ss << std::to_string(double(ms.count()) / 1000).substr(2, 3) << " sec";

    return ss.str();
}

/// ***
/// Get string representation of the current system time
/// ***
std::string scan::Timer::timestamp() const
{
    return timestamp(system_now());
}
