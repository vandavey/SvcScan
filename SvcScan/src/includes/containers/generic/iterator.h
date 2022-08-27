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

        using difference_type   = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept  = iterator_category;

    private:  /* Fields */
        const value_type *m_valuep;  // Element pointer

    public:  /* Constructors & Destructor */
        Iterator();
        Iterator(const Iterator &t_it);
        Iterator(Iterator &&) = default;
        Iterator(const value_type *t_valuep);

        virtual ~Iterator() = default;

    public:  /* Operators */
        Iterator &operator=(const Iterator &) = default;
        Iterator &operator=(Iterator &&) = default;

        operator size_t() const;

        const T *operator->() const;
        const T &operator*() const;

        Iterator operator+(const size_t &t_idx) const;
        Iterator operator+(const int &t_idx) const;

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
    m_valuep = nullptr;
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator(const Iterator &t_it)
{
    m_valuep = t_it.m_valuep;
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::Iterator<T>::Iterator(const value_type *t_valuep)
{
    m_valuep = t_valuep;
}

/**
* @brief  Cast operator overload.
*/
template<class T>
inline scan::Iterator<T>::operator size_t() const
{
    return reinterpret_cast<size_t>(m_valuep);
}

/**
* @brief  Dereference operator overload.
*/
template<class T>
inline const T *scan::Iterator<T>::operator->() const
{
    return m_valuep;
}

/**
* @brief  Indirection operator overload.
*/
template<class T>
inline const T &scan::Iterator<T>::operator*() const
{
    return *m_valuep;
}

/**
* @brief  Addition operator overload.
*/
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator+(const size_t &t_idx) const
{
    return static_cast<Iterator>(m_valuep + t_idx);
}

/**
* @brief  Addition operator overload.
*/
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator+(const int &t_idx) const
{
    return *this + static_cast<size_t>(t_idx);
}

/**
* @brief  Prefix increment operator overload.
*/
template<class T>
inline scan::Iterator<T> &scan::Iterator<T>::operator++()
{
    m_valuep++;
    return *this;
}

/**
* @brief  Postfix increment operator overload.
*/
template<class T>
inline scan::Iterator<T> scan::Iterator<T>::operator++(int)
{
    const Iterator orig{ *this };
    ++(*this);
    return orig;
}

#endif // !ITERATOR_H
