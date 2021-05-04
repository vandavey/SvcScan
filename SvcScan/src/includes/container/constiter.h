/*
*  constiter.h
*  -----------
*  Header file for constant container forward iterator
*/
#pragma once

#include "iterator.h"

#ifndef CONST_ITER
#define CONST_ITER

namespace scan
{
    /// ***
    /// Constant forward iterator for generic containers
    /// ***
    template<class T>
    class ConstIter : public Iterator<T>
    {
    public:  /* Types */
        using base = Iterator<T>;

        using pointer    = typename base::pointer;
        using reference  = typename base::reference;
        using value_type = typename base::value_type;

        using difference_type   = typename base::difference_type;
        using iterator_category = typename base::iterator_category;

    private:  /* Fields */
        const value_type *m_ptr;  // Element pointer

    public:  /* Constructors & Destructor */
        ConstIter();
        ConstIter(const ConstIter &t_cit);
        ConstIter(const value_type *t_ptr);

        virtual ~ConstIter() = default;

    public:  /* Operators */
        operator size_t() const;

        const T *operator->() const;
        const T &operator*() const;

        ConstIter operator+(const size_t &t_idx) const;
        ConstIter operator+(const int &t_idx) const;

        ConstIter &operator++();
        ConstIter operator++(int);

        bool operator==(const ConstIter &t_it) const noexcept;
        bool operator!=(const ConstIter &t_it) const noexcept;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::ConstIter<T>::ConstIter()
{
    m_ptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::ConstIter<T>::ConstIter(const ConstIter &t_it)
{
    m_ptr = t_it.m_ptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::ConstIter<T>::ConstIter(const value_type *t_ptr)
{
    m_ptr = t_ptr;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::ConstIter<T>::operator size_t() const
{
    return reinterpret_cast<size_t>(m_ptr);
}

/// ***
/// Dereference operator overload
/// ***
template<class T>
inline const T *scan::ConstIter<T>::operator->() const
{
    return m_ptr;
}

/// ***
/// Indirection operator overload
/// ***
template<class T>
inline const T &scan::ConstIter<T>::operator*() const
{
    return *m_ptr;
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline scan::ConstIter<T> scan::ConstIter<T>::operator+(const size_t &t_idx)
const {
    return static_cast<ConstIter>(m_ptr + t_idx);
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline scan::ConstIter<T> scan::ConstIter<T>::operator+(const int &t_idx) const
{
    return operator+(static_cast<size_t>(t_idx));
}

/// ***
/// Prefix increment operator overload
/// ***
template<class T>
inline scan::ConstIter<T> &scan::ConstIter<T>::operator++()
{
    m_ptr++;
    return *this;
}

/// ***
/// Postfix increment operator overload
/// ***
template<class T>
inline scan::ConstIter<T> scan::ConstIter<T>::operator++(int)
{
    const ConstIter orig{ *this };
    ++(*this);
    return orig;
}

/// ***
/// Equality operator overload
/// ***
template<class T>
inline bool scan::ConstIter<T>::operator==(const ConstIter &t_it) const noexcept
{
    return m_ptr == t_it.m_ptr;
}

/// ***
/// Inequality operator overload
/// ***
template<class T>
inline bool scan::ConstIter<T>::operator!=(const ConstIter &t_it) const noexcept
{
    return m_ptr != t_it.m_ptr;
}

#endif // !CONST_ITER
