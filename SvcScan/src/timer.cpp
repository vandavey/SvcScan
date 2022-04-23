/*
*  timer.cpp
*  ---------
*  Source file for timer using steady clock
*/
#include <iomanip>
#include "includes/utils/timer.h"
#include "includes/utils/util.h"

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
/// Utility - Retrieve the current time as a 'steady_clock' time point
/// ***
scan::Timer::steady_tp scan::Timer::steady_now() noexcept
{
    return steady_clock::now();
}

/// ***
/// Utility - Retrieve the current time as a 'system_clock' time point
/// ***
scan::Timer::system_tp scan::Timer::system_now() noexcept
{
    return system_clock::now();
}

/// ***
/// Utility - Format the given time point as a date-time in the specified format
/// ***
std::string scan::Timer::timestamp(const system_tp &t_tp, const string &t_dt_fmt)
{
    std::stringstream ss;
    const time_t tt{ system_clock::to_time_t(t_tp) };

    tm time{ 0 };
    localtime_s(&time, &tt);

    ss << std::put_time(&time, t_dt_fmt.c_str());

    return ss.str();
}

/// ***
/// Determine whether the timer has started and is currently running
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
    std::stringstream ss;
    milliseconds ms{ elapsed() };

    // Calculate duration in hours
    if (ms >= chrono::hours(1))
    {
        const chrono::hours hr_floor{ chrono::floor<chrono::hours>(ms) };
        ms -= hr_floor;

        const string hr_noun{ (hr_floor < chrono::hours(2)) ? "hour" : "hours" };
        ss << Util::fstr("% %, ", hr_floor.count(), hr_noun);
    }

    const chrono::minutes min_floor{ chrono::floor<chrono::minutes>(ms) };
    ms -= min_floor;

    const chrono::seconds sec_floor{ chrono::floor<chrono::seconds>(ms) };
    ms -= sec_floor;

    // Get the duration as a fraction of seconds
    const string sec_fraction{ std::to_string(double(ms.count()) / 1000) };

    // Interpolate the durations values
    ss << Util::fstr("% min, %.% sec",
                     min_floor.count(),
                     sec_floor.count(),
                     sec_fraction.substr(2, 3));

    return ss.str();
}

/// ***
/// Format the current system date-time using the specified format
/// ***
std::string scan::Timer::timestamp() const
{
    return timestamp(system_now());
}
