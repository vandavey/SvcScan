/*
*  range.h
*  -------
*  Header file for integral range container
*/
#pragma once

#ifndef RANGE_H
#define RANGE_H

#include <vector>
#include "../../except/argex.h"
#include "citerator.h"

namespace scan
{
    /// ***
    /// Integral range container service information
    /// ***
    template<class T>
    class Range
    {
    public:  /* Types */
        using value_type     = T;
        using const_iterator = CIterator<value_type>;

    private:  /* Types */
        using vector_t = std::vector<value_type>;

    private:  /* Fields */
        value_type m_min;  // Minimum value
        value_type m_max;  // Maximum value

        vector_t m_vect;   // Underlying container

    public:  /* Constructors & Destructor */
        Range(const Range &t_range);
        Range(const value_type &t_min, const value_type &t_max);

        virtual ~Range() = default;

    private:  /* Constructors (deleted) */
        Range() = delete;

    public:  /* Operators */
        operator vector_t() const noexcept;

    private: /* Assertions */
        static_assert(std::is_integral_v<T>, "class 'T' must be an integral type");

    public:  /* Methods */
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        vector_t values() const noexcept;

    private:  /* Methods */
        void update(const value_type &t_min, const value_type &t_max);
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Range<T>::Range(const Range &t_range)
{
    m_min = t_range.m_min;
    m_max = t_range.m_max;
    m_vect = t_range.m_vect;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Range<T>::Range(const value_type &t_min,
                             const value_type &t_max) : m_min(t_min), m_max(t_max) {
    if (t_max <= t_min)
    {
        throw ArgEx({ "t_min", "t_max" }, "Maximum must be greater than minimum");
    }
    update(t_min, t_max);
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::Range<T>::operator vector_t() const noexcept
{
    return values();
}

/// ***
/// Get iterator to first element in underlying vector
/// ***
template<class T>
inline typename scan::Range<T>::const_iterator scan::Range<T>::begin() const noexcept
{
    return static_cast<const_iterator>(m_vect.data());
}

/// ***
/// Get iterator to past-the-end element in underlying vector
/// ***
template<class T>
inline typename scan::Range<T>::const_iterator scan::Range<T>::end() const noexcept
{
    return static_cast<const_iterator>(m_vect.data() + m_vect.size());
}

/// ***
/// Retrieve values from the underlying container
/// ***
template<class T>
inline typename scan::Range<T>::vector_t scan::Range<T>::values() const noexcept
{
    return m_vect;
}

/// ***
/// Refresh underlying container based on current limits (min/max)
/// ***
template<class T>
inline void scan::Range<T>::update(const value_type &t_min, const value_type &t_max)
{
    if (t_max <= t_min)
    {
        throw ArgEx({ "t_min", "t_max" }, "Maximum must be greater than minimum");
    }

    m_vect.clear();
    m_vect.shrink_to_fit();

    // Add elements (including m_max)
    for (value_type i{ m_min }; i <= m_max; i++)
    {
        m_vect.push_back(i);
    }
}

#endif // !RANGE_H
