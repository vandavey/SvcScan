/*
*  iterator.h
*  ----------
*  Header file for generic container iterator
*/
#pragma once

#ifndef ITERATOR_H
#define ITERATOR_H

namespace Scan
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
        Iterator(const Iterator &it);
        Iterator(const value_type *ptr);

        virtual ~Iterator() = default;

    public:  /* Operators */
        operator const size_t() const;

        const T *operator->() const;
        const T &operator*() const;

        Iterator operator+(const size_t &idx) const;
        Iterator operator+(const int &idx) const;

        Iterator &operator++();
        Iterator operator++(int);

        const bool operator==(const Iterator &it) const noexcept;
        const bool operator!=(const Iterator &it) const noexcept;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Iterator<T>::Iterator()
{
    this->m_ptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Iterator<T>::Iterator(const Iterator &it)
{
    this->m_ptr = it.m_ptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::Iterator<T>::Iterator(const value_type *ptr)
{
    this->m_ptr = ptr;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline Scan::Iterator<T>::operator const size_t() const
{
    return reinterpret_cast<size_t>(m_ptr);
}

/// ***
/// Dereference operator overload
/// ***
template<class T>
inline const T *Scan::Iterator<T>::operator->() const
{
    return m_ptr;
}

/// ***
/// Indirection operator overload
/// ***
template<class T>
inline const T &Scan::Iterator<T>::operator*() const
{
    return *m_ptr;
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline Scan::Iterator<T> Scan::Iterator<T>::operator+(const size_t &idx) const
{
    return static_cast<Iterator>(m_ptr + idx);
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline Scan::Iterator<T> Scan::Iterator<T>::operator+(const int &idx) const
{
    return operator+(static_cast<size_t>(idx));
}

/// ***
/// Prefix increment operator overload
/// ***
template<class T>
inline Scan::Iterator<T> &Scan::Iterator<T>::operator++()
{
    m_ptr++;
    return *this;
}

/// ***
/// Postfix increment operator overload
/// ***
template<class T>
inline Scan::Iterator<T> Scan::Iterator<T>::operator++(int)
{
    Iterator orig(*this);
    ++(*this);
    return orig;
}

/// ***
/// Equality operator overload
/// ***
template<class T>
inline const bool Scan::Iterator<T>::operator==(const Iterator &it) const
noexcept {
    return m_ptr == it.m_ptr;
}

/// ***
/// Inequality operator overload
/// ***
template<class T>
inline const bool Scan::Iterator<T>::operator!=(const Iterator &it) const
noexcept {
    return m_ptr != it.m_ptr;
}

#endif // !ITERATOR_H
