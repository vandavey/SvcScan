/*
*  iterator.h
*  ----------
*  Header file for a generic container constant (forward) iterator
*/
#pragma once

#ifndef ITERATOR_H
#define ITERATOR_H

#include <compare>
#include <iterator>
#include "../../except/logic_ex.h"

namespace scan
{
    /**
    * @brief  Constant forward iterator for generic containers.
    */
    template<class T>
    class Iterator final
    {
    public:  /* Type Aliases */
        using value_type = T;
        using pointer    = const value_type *;
        using reference  = const value_type &;

        using iterator_category = std::forward_iterator_tag;
        using iterator_concept  = iterator_category;

    private:  /* Type Aliases */
        using this_t = Iterator;

    private:  /* Fields */
        const value_type *m_pointer;  // Value type pointer

    public:  /* Constructors & Destructor */
        Iterator();
        Iterator(const Iterator &t_iter);
        Iterator(Iterator &&) = default;
        Iterator(const value_type *t_pointer);

        virtual ~Iterator() = default;

    public:  /* Operators */
        Iterator &operator=(const Iterator &) = default;
        Iterator &operator=(Iterator &&) = default;

        operator uintptr_t() const noexcept;

        const T *operator->() const noexcept;
        const T &operator*() const;

        Iterator operator+(const uintptr_t &t_addr) const;

        Iterator &operator++();
        Iterator operator++(int);

        std::strong_ordering operator<=>(const Iterator &) const = default;
    };
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator()
{
    m_pointer = nullptr;
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator(const Iterator &t_iter)
{
    m_pointer = t_iter.m_pointer;
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator(const value_type *t_pointer)
{
    m_pointer = t_pointer;
}

/**
* @brief  Cast operator overload.
*/
template<class T>
inline scan::Iterator<T>::operator uintptr_t() const noexcept
{
    return static_cast<uintptr_t>(m_pointer);
}

/**
* @brief  Dereference operator overload.
*/
template<class T>
inline const T *scan::Iterator<T>::operator->() const noexcept
{
    return m_pointer;
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
    return *m_pointer;
}

/**
* @brief  Addition operator overload.
*/
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator+(const uintptr_t &t_addr) const
{
    return static_cast<this_t>(m_pointer + t_addr);
}

/**
* @brief  Prefix increment operator overload.
*/
template<class T>
inline scan::Iterator<T> &scan::Iterator<T>::operator++()
{
    m_pointer++;
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
