/*
* @file
*     iterator.h
* @brief
*     Header file for a generic constant forward iterator.
*/
#pragma once

#ifndef ITERATOR_H
#define ITERATOR_H

#include <compare>
#include <iterator>
#include "../../errors/logic_ex.h"
#include "../../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Generic constant forward iterator.
    */
    template<class T>
    class Iterator final
    {
    public:  /* Type Aliases */
        using value_type = T;
        using pointer    = const value_type *;
        using reference  = const value_type &;

        using difference_type   = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept  = std::forward_iterator_tag;

    private:  /* Type Aliases */
        using this_t = Iterator;

    private:  /* Fields */
        const value_type *m_ptr;  // Value type pointer

    public:  /* Constructors & Destructor */
        Iterator() noexcept;
        Iterator(const Iterator &t_iter) noexcept;
        Iterator(Iterator &&) = default;
        Iterator(const value_type *t_ptr) noexcept;

        virtual ~Iterator() = default;

    public:  /* Operators */
        Iterator &operator=(const Iterator &) = default;
        Iterator &operator=(Iterator &&) = default;

        operator uintptr_t() const noexcept;

        const T *operator->() const noexcept;
        const T &operator*() const;

        ptrdiff_t operator-(const Iterator &t_iter) const;
        Iterator operator+(const uintptr_t &t_n) const;

        Iterator &operator++();
        Iterator operator++(int);

        std::strong_ordering operator<=>(const Iterator &) const = default;
    };
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator() noexcept
{
    m_ptr = nullptr;
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator(const Iterator &t_iter) noexcept
{
    m_ptr = t_iter.m_ptr;
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator(const value_type *t_ptr) noexcept
{
    m_ptr = t_ptr;
}

/**
* @brief  Cast operator overload.
*/
template<class T>
inline scan::Iterator<T>::operator uintptr_t() const noexcept
{
    return static_cast<uintptr_t>(m_ptr);
}

/**
* @brief  Dereference operator overload.
*/
template<class T>
inline const T *scan::Iterator<T>::operator->() const noexcept
{
    return m_ptr;
}

/**
* @brief  Indirection operator overload.
*/
template<class T>
inline const T &scan::Iterator<T>::operator*() const
{
    if (*this == nullptr)
    {
        throw LogicEx{ "Iterator<T>::operator*", "Null pointer dereferenced" };
    }
    return *m_ptr;
}

/**
* @brief  Subtraction operator overload.
*/
template<class T>
inline ptrdiff_t scan::Iterator<T>::operator-(const Iterator &t_iter) const
{
    return m_ptr - t_iter.m_ptr;
}

/**
* @brief  Addition operator overload.
*/
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator+(const uintptr_t &t_n) const
{
    return static_cast<this_t>(m_ptr + t_n);
}

/**
* @brief  Prefix increment operator overload.
*/
template<class T>
inline scan::Iterator<T> &scan::Iterator<T>::operator++()
{
    m_ptr++;
    return *this;
}

/**
* @brief  Postfix increment operator overload.
*/
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator++(int)
{
    const this_t iter{ *this };
    ++(*this);

    return iter;
}

#endif // !ITERATOR_H
