/*
*  iterator.h
*  ----------
*  Header file for generic container iterator
*/
#pragma once

#ifndef ITERATOR_H
#define ITERATOR_H

namespace scan
{
    /// ***
    /// Constant forward iterator for containers
    /// ***
    template<class T>
    class Iterator
    {
    public:  /* Types */
        using value_type = T;
        using pointer = typename value_type *;
        using reference = typename value_type &;

        using size_type = size_t;
        using difference_type = ptrdiff_t;

    private:  /* Fields */
        const value_type *m_ptr;  // Element pointer

    public:  /* Constructors & Destructor */
        Iterator();
        Iterator(const Iterator &t_it);
        Iterator(const value_type *t_ptr);

        virtual ~Iterator() = default;

    public:  /* Operators */
        operator const size_t() const;

        const T *operator->() const;
        const T &operator*() const;

        Iterator operator+(const size_t &t_idx) const;
        Iterator operator+(const int &t_idx) const;

        Iterator &operator++();
        Iterator operator++(int);

        const bool operator==(const Iterator &t_it) const noexcept;
        const bool operator!=(const Iterator &t_it) const noexcept;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Iterator<T>::Iterator()
{
    m_ptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Iterator<T>::Iterator(const Iterator &t_it)
{
    m_ptr = t_it.m_ptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Iterator<T>::Iterator(const value_type *t_ptr)
{
    m_ptr = t_ptr;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::Iterator<T>::operator const size_t() const
{
    return reinterpret_cast<size_t>(m_ptr);
}

/// ***
/// Dereference operator overload
/// ***
template<class T>
inline const T *scan::Iterator<T>::operator->() const
{
    return m_ptr;
}

/// ***
/// Indirection operator overload
/// ***
template<class T>
inline const T &scan::Iterator<T>::operator*() const
{
    return *m_ptr;
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator+(const size_t &t_idx) const
{
    return static_cast<Iterator>(m_ptr + t_idx);
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator+(const int &t_idx) const
{
    return operator+(static_cast<size_t>(t_idx));
}

/// ***
/// Prefix increment operator overload
/// ***
template<class T>
inline scan::Iterator<T> &scan::Iterator<T>::operator++()
{
    m_ptr++;
    return *this;
}

/// ***
/// Postfix increment operator overload
/// ***
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator++(int)
{
    const Iterator orig{ *this };
    ++(*this);
    return orig;
}

/// ***
/// Equality operator overload
/// ***
template<class T>
inline const bool scan::Iterator<T>::operator==(const Iterator &t_it) const
noexcept {
    return m_ptr == t_it.m_ptr;
}

/// ***
/// Inequality operator overload
/// ***
template<class T>
inline const bool scan::Iterator<T>::operator!=(const Iterator &t_it) const
noexcept {
    return m_ptr != t_it.m_ptr;
}

#endif // !ITERATOR_H
