/*
*  timeout.h
*  ---------
*  Source file for a socket timeout duration
*/
#include <cmath>
#include "includes/inet/sockets/timeout.h"

/// ***
/// Initialize the object
/// ***
scan::Timeout::Timeout()
{
    m_milli = chrono::milliseconds::zero();
}

/// ***
/// Initialize the object
/// ***
scan::Timeout::Timeout(const Timeout &t_timeout)
{
    m_milli = t_timeout.m_milli;
}

/// ***
/// Initialize the object
/// ***
scan::Timeout::Timeout(const uint &t_milli)
{
    operator=(t_milli);
}

/// ***
/// Assignment operator overload
/// ***
scan::Timeout &scan::Timeout::operator=(const Timeout &t_timeout)
{
    m_milli = t_timeout.m_milli;
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
scan::Timeout &scan::Timeout::operator=(const uint &t_milli)
{
    m_milli = milliseconds(t_milli);
    return *this;
}

/// ***
/// Assignment operator overload
/// ***
scan::Timeout &scan::Timeout::operator=(const milliseconds &t_milli)
{
    m_milli = t_milli;
    return *this;
}

/// ***
/// Cast operator overload
/// ***
scan::Timeout::operator uint() const noexcept
{
    return static_cast<uint>(m_milli.count());
}

/// ***
/// Cast operator overload
/// ***
scan::Timeout::operator milliseconds() const noexcept
{
    return m_milli;
}

/// ***
/// Cast operator overload
/// ***
scan::Timeout::operator timeval() const
{
    const double total_sec{ double(m_milli.count()) / 1000 };
    const int sec_floor{ static_cast<int>(std::floor(total_sec)) };

    const seconds sec{ static_cast<int>(total_sec) };
    const milliseconds milli{ static_cast<int>((total_sec - sec_floor) * 1000) };

    return timeval  // Timeout time value
    {
        static_cast<long>(sec.count()),
        static_cast<long>(chrono::duration_cast<microseconds>(milli).count())
    };
}
