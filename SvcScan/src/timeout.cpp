/*
*  timeout.cpp
*  -----------
*  Source file for a network socket timeout
*/
#include <cmath>
#include "includes/inet/sockets/timeout.h"

/**
* @brief  Initialize the object.
*/
scan::Timeout::Timeout()
{
    m_milli = chrono::milliseconds::zero();
}

/**
* @brief  Initialize the object.
*/
scan::Timeout::Timeout(const Timeout &t_timeout)
{
    m_milli = t_timeout.m_milli;
}

/**
* @brief  Initialize the object.
*/
scan::Timeout::Timeout(const uint &t_milli)
{
    operator=(t_milli);
}

/**
* @brief  Assignment operator overload.
*/
scan::Timeout &scan::Timeout::operator=(const Timeout &t_timeout)
{
    m_milli = t_timeout.m_milli;
    return *this;
}

/**
* @brief  Assignment operator overload.
*/
scan::Timeout &scan::Timeout::operator=(const uint &t_milli)
{
    m_milli = milliseconds(t_milli);
    return *this;
}

/**
* @brief  Assignment operator overload.
*/
scan::Timeout &scan::Timeout::operator=(const milliseconds &t_milli)
{
    m_milli = t_milli;
    return *this;
}

/**
* @brief  Cast operator overload.
*/
scan::Timeout::operator uint() const noexcept
{
    return static_cast<uint>(m_milli.count());
}

/**
* @brief  Cast operator overload.
*/
scan::Timeout::operator milliseconds() const noexcept
{
    return m_milli;
}

/**
* @brief  Cast operator overload.
*/
scan::Timeout::operator timeval() const
{
    const double total_sec{ double(m_milli.count()) / 1000 };
    const int sec_floor{ static_cast<int>(std::floor(total_sec)) };

    const seconds sec{ static_cast<uint>(total_sec) };
    const milliseconds milli{ static_cast<uint>((total_sec - sec_floor) * 1000) };
    const microseconds micro{ chrono::duration_cast<microseconds>(milli) };

    return timeval
    {
        .tv_sec  = static_cast<long>(sec.count()),
        .tv_usec = static_cast<long>(micro.count())
    };
}

/**
* @brief  Equality operator overload.
*/
bool scan::Timeout::operator==(const Timeout &t_timeout) const noexcept
{
    return operator milliseconds() == static_cast<milliseconds>(t_timeout);
}

/**
* @brief  Inequality operator overload.
*/
bool scan::Timeout::operator!=(const Timeout &t_timeout) const noexcept
{
    return operator milliseconds() != static_cast<milliseconds>(t_timeout);
}
