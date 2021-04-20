/*
*  iterator.h
*  ----------
*  Header file for generic container forward iterator
*/
#pragma once

#ifndef ITERATOR_H
#define ITERATOR_H

namespace scan
{
    /// ***
    /// Forward iterator for generic containers
    /// ***
    template<class T>
    class Iterator
    {
    public:  /* Types */
        using value_type = T;
        using pointer = typename value_type *;
        using reference = typename value_type &;

        using difference_type = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

    private:  /* Fields */
        value_type *m_ptr;  // Element pointer

    public:  /* Constructors & Destructor */
        Iterator();
        Iterator(const Iterator &t_it);
        Iterator(value_type *t_ptr);

        virtual ~Iterator() = default;

    public:  /* Operators */
        operator size_t() const;

        T *operator->();
        T &operator*();

        Iterator operator+(const size_t &t_idx) const;
        Iterator operator+(const int &t_idx) const;

        Iterator &operator++();
        Iterator operator++(int);

        bool operator==(const Iterator &t_it) const noexcept;
        bool operator!=(const Iterator &t_it) const noexcept;
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
inline scan::Iterator<T>::Iterator(value_type *t_ptr)
{
    m_ptr = t_ptr;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::Iterator<T>::operator size_t() const
{
    return reinterpret_cast<size_t>(m_ptr);
}

/// ***
/// Dereference operator overload
/// ***
template<class T>
inline T *scan::Iterator<T>::operator->()
{
    return m_ptr;
}

/// ***
/// Indirection operator overload
/// ***
template<class T>
inline T &scan::Iterator<T>::operator*()
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
inline bool scan::Iterator<T>::operator==(const Iterator &t_it) const noexcept
{
    return m_ptr == t_it.m_ptr;
}

/// ***
/// Inequality operator overload
/// ***
template<class T>
inline bool scan::Iterator<T>::operator!=(const Iterator &t_it) const noexcept
{
    return m_ptr != t_it.m_ptr;
}

#endif // !ITERATOR_H
