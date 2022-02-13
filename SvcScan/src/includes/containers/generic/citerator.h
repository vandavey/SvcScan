/*
*  citerator.h
*  -----------
*  Header file for a generic constant forward iterator
*/
#pragma once

#ifndef C_ITERATOR_H
#define C_ITERATOR_H

namespace scan
{
    /// ***
    /// Constant forward iterator for generic containers
    /// ***
    template<class T>
    class CIterator final
    {
    public:  /* Type Aliases */
        using value_type = T;
        using pointer    = typename value_type *;
        using reference  = typename value_type &;

        using difference_type   = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

    private:  /* Fields */
        const value_type *m_vptr;  // Element pointer

    public:  /* Constructors & Destructor */
        CIterator();
        CIterator(const CIterator &t_it);
        CIterator(const value_type *t_ptr);

        virtual ~CIterator() = default;

    public:  /* Operators */
        operator size_t() const;

        const T *operator->() const;
        const T &operator*() const;

        CIterator operator+(const size_t &t_idx) const;
        CIterator operator+(const int &t_idx) const;

        CIterator &operator++();
        CIterator operator++(int);

        bool operator==(const CIterator &t_it) const noexcept;
        bool operator!=(const CIterator &t_it) const noexcept;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::CIterator<T>::CIterator()
{
    m_vptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::CIterator<T>::CIterator(const CIterator &t_it)
{
    m_vptr = t_it.m_vptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::CIterator<T>::CIterator(const value_type *t_ptr)
{
    m_vptr = t_ptr;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::CIterator<T>::operator size_t() const
{
    return reinterpret_cast<size_t>(m_vptr);
}

/// ***
/// Dereference operator overload
/// ***
template<class T>
inline const T *scan::CIterator<T>::operator->() const
{
    return m_vptr;
}

/// ***
/// Indirection operator overload
/// ***
template<class T>
inline const T &scan::CIterator<T>::operator*() const
{
    return *m_vptr;
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline scan::CIterator<T> scan::CIterator<T>::operator+(const size_t &t_idx) const
{
    return static_cast<CIterator>(m_vptr + t_idx);
}

/// ***
/// Addition operator overload
/// ***
template<class T>
inline scan::CIterator<T> scan::CIterator<T>::operator+(const int &t_idx) const
{
    return operator+(static_cast<size_t>(t_idx));
}

/// ***
/// Prefix increment operator overload
/// ***
template<class T>
inline scan::CIterator<T> &scan::CIterator<T>::operator++()
{
    m_vptr++;
    return *this;
}

/// ***
/// Postfix increment operator overload
/// ***
template<class T>
inline scan::CIterator<T> scan::CIterator<T>::operator++(int)
{
    const CIterator orig{ *this };
    ++(*this);
    return orig;
}

/// ***
/// Equality operator overload
/// ***
template<class T>
inline bool scan::CIterator<T>::operator==(const CIterator &t_it) const noexcept
{
    return m_vptr == t_it.m_vptr;
}

/// ***
/// Inequality operator overload
/// ***
template<class T>
inline bool scan::CIterator<T>::operator!=(const CIterator &t_it) const noexcept
{
    return m_vptr != t_it.m_vptr;
}

#endif // !C_ITERATOR_H
