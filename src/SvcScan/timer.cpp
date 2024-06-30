/*
* @file
*     timer.cpp
* @brief
*     Source file for a steady-clock timer.
*/
#include <format>
#include <string>
#include "includes/utils/timer.h"

/**
* @brief
*     Create a timestamp from the given time point.
*/
std::string scan::Timer::timestamp(const system_time_point &t_time)
{
    const string zone_name{ chrono::current_zone()->name() };
    return algo::fstr("% %", std::format(STAMP_FMT, t_time), zone_name);
}

/**
* @brief
*     Create a timestamp from the underlying end time point.
*/
std::string scan::Timer::end_time() const
{
    return timestamp(end_time_point());
}

/**
* @brief
*     Start the timer and create a timestamp from the underlying start time point.
*/
std::string scan::Timer::start()
{
    m_start_time = steady_clock::now();
    return timestamp(m_sys_start_time = system_clock::now());
}

/**
* @brief
*     Create a timestamp from the underlying start time point.
*/
std::string scan::Timer::start_time() const
{
    return timestamp(start_time_point());
}

/**
* @brief
*     Stop the timer and create a timestamp from the underlying end time point.
*/
std::string scan::Timer::stop()
{
    m_end_time = steady_clock::now();
    return timestamp(m_sys_end_time = system_clock::now());
}
