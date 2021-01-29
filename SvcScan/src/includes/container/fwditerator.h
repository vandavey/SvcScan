/*
*  fwditerator.h
*  -------------
*  Header file for generic container iterator
*/
#pragma once

#ifndef FWDITERATOR_H
#define FWDITERATOR_H

namespace Scan
{
    /// ***
    /// Constant forward iterator for containers
    /// ***
    template<class T>
    class FwdIterator
    {
    public:  /* Types */
        using pointer = T*;
        using reference = T&;
        using value_type = T;
        using difference_type = ptrdiff_t;

    private:  /* Fields */
        const T *m_ptr;  // Element pointer

    public:  /* Constructors & Destructor */
        FwdIterator();
        FwdIterator(const FwdIterator &it);
        explicit FwdIterator(const T *ptr);

        virtual ~FwdIterator() = default;

    public:  /* Operators */
        const T *operator->() const;
        const T &operator*() const;

        FwdIterator &operator++();
        FwdIterator operator++(int);

        const bool operator==(const FwdIterator &it) const noexcept;
        const bool operator!=(const FwdIterator &it) const noexcept;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::FwdIterator<T>::FwdIterator()
{
    this->m_ptr = nullptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::FwdIterator<T>::FwdIterator(const FwdIterator &it)
{
    this->m_ptr = it.m_ptr;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::FwdIterator<T>::FwdIterator(const T *ptr)
{
    this->m_ptr = ptr;
}

/// ***
/// Dereference operator overload
/// ***
template<class T>
inline const T *Scan::FwdIterator<T>::operator->() const
{
    return static_cast<const T *>(m_ptr);
}

/// ***
/// Indirection operator overload
/// ***
template<class T>
inline const T &Scan::FwdIterator<T>::operator*() const
{
    return static_cast<const T &>(*m_ptr);
}

/// ***
/// Prefix increment operator overload
/// ***
template<class T>
inline Scan::FwdIterator<T> &Scan::FwdIterator<T>::operator++()
{
    m_ptr++;
    return *this;
}

/// ***
/// Postfix increment operator overload
/// ***
template<class T>
inline Scan::FwdIterator<T> Scan::FwdIterator<T>::operator++(int)
{
    FwdIterator prev = {*this};
    ++(*this);
    return prev;
}

/// ***
/// Equality operator overload
/// ***
template<class T>
inline const bool Scan::FwdIterator<T>::operator==(const FwdIterator &it) const
noexcept {
    return (m_ptr == it.m_ptr);
}

/// ***
/// Inequality operator overload
/// ***
template<class T>
inline const bool Scan::FwdIterator<T>::operator!=(const FwdIterator &it) const
noexcept {
    return (m_ptr != it.m_ptr);
}

#endif // !FWDITERATOR_H
