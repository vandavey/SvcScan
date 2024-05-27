/*
* @file
*     timeout.cpp
* @brief
*     Source file for a network socket timeout.
*/
#include <cmath>
#include "includes/inet/sockets/timeout.h"

/**
* @brief  Initialize the object.
*/
scan::Timeout::Timeout() noexcept
{
    m_milli = milliseconds::zero();
}

/**
* @brief  Initialize the object.
*/
scan::Timeout::Timeout(const Timeout &t_timeout) noexcept
{
    *this = t_timeout;
}

/**
* @brief  Initialize the object.
*/
scan::Timeout::Timeout(const uint_t &t_milli) noexcept
{
    *this = t_milli;
}

/**
* @brief  Copy assignment operator overload.
*/
scan::Timeout &scan::Timeout::operator=(const Timeout &t_timeout) noexcept
{
    m_milli = t_timeout.m_milli;
    return *this;
}

/**
* @brief  Assignment operator overload.
*/
scan::Timeout &scan::Timeout::operator=(const uint_t &t_milli) noexcept
{
    m_milli = milliseconds(t_milli);
    return *this;
}

/**
* @brief  Assignment operator overload.
*/
scan::Timeout &scan::Timeout::operator=(const milliseconds &t_milli) noexcept
{
    m_milli = t_milli;
    return *this;
}

/**
* @brief  Cast operator overload.
*/
scan::Timeout::operator scan::uint_t() const noexcept
{
    return static_cast<uint_t>(m_milli.count());
}

/**
* @brief  Cast operator overload.
*/
scan::Timeout::operator scan::milliseconds() const noexcept
{
    return m_milli;
}

/**
* @brief  Cast operator overload.
*/
scan::Timeout::operator timeval() const
{
    const double total_sec{ static_cast<double>(m_milli.count()) / 1000 };
    const int sec_floor{ static_cast<int>(std::floor(total_sec)) };

    const seconds sec{ static_cast<uint_t>(total_sec) };
    const milliseconds milli{ static_cast<uint_t>((total_sec - sec_floor) * 1000) };
    const microseconds micro{ chrono::duration_cast<microseconds>(milli) };

    return timeval
    {
        .tv_sec  = static_cast<long>(sec.count()),
        .tv_usec = static_cast<long>(micro.count())
    };
}
