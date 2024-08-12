/*
* @file
*     const_iterator.h
* @brief
*     Header file for a generic constant bidirectional iterator.
*/
#pragma once

#ifndef SCAN_CONST_ITERATOR_H
#define SCAN_CONST_ITERATOR_H

#include <concepts>
#include <cstdint>
#include "../utils/aliases.h"
#include "iterator_traits.h"

namespace scan
{
    /**
    * @brief
    *     Generic constant bidirectional iterator.
    */
    template<class T>
    class ConstIterator
    {
    public:  /* Type Aliases */
        using value_type      = IteratorTraits<T>::value_type;
        using pointer         = const value_type*;
        using reference       = const value_type&;
        using difference_type = IteratorTraits<T>::difference_type;

        using iterator_category = IteratorTraits<T>::iterator_category;
        using iterator_concept  = IteratorTraits<T>::iterator_concept;

    private:  /* Fields */
        const value_type* m_ptr;  // Value type pointer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr ConstIterator() noexcept : ConstIterator{nullptr}
        {
        }

        constexpr ConstIterator(const ConstIterator&) = default;
        constexpr ConstIterator(ConstIterator&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr ConstIterator(const value_type* t_ptr) noexcept
        {
            m_ptr = t_ptr;
        }

        virtual constexpr ~ConstIterator() = default;

    public:  /* Operators */
        constexpr ConstIterator& operator=(const ConstIterator&) = default;
        constexpr ConstIterator& operator=(ConstIterator&&) = default;

        /**
        * @brief
        *     Addition assignment operator overload.
        */
        template<std::integral D>
        constexpr ConstIterator& operator+=(D t_offset) noexcept
        {
            m_ptr += static_cast<ptrdiff_t>(t_offset);
            return *this;
        }

        /**
        * @brief
        *     Subtraction assignment operator overload.
        */
        template<std::integral D>
        constexpr ConstIterator& operator-=(D t_offset) noexcept
        {
            m_ptr -= static_cast<ptrdiff_t>(t_offset);
            return *this;
        }

        constexpr strong_ordering operator<=>(const ConstIterator&) const = default;

        /**
        * @brief
        *     Dereference operator overload.
        */
        constexpr const value_type* operator->() const noexcept
        {
            return m_ptr;
        }

        /**
        * @brief
        *     Indirection operator overload.
        */
        constexpr const value_type& operator*() const noexcept
        {
            return *m_ptr;
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr const value_type& operator[](ptrdiff_t t_index) const noexcept
        {
            return m_ptr[t_index];
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        template<std::integral D>
        constexpr ConstIterator operator+(D t_offset) const noexcept
        {
            return ConstIterator{m_ptr + static_cast<ptrdiff_t>(t_offset)};
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        constexpr ConstIterator operator+(ConstIterator t_iter) const noexcept
        {
            return ConstIterator{m_ptr + static_cast<ptrdiff_t>(t_iter)};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        template<std::integral D>
        constexpr ConstIterator operator-(D t_offset) const noexcept
        {
            return ConstIterator{m_ptr - static_cast<ptrdiff_t>(t_offset)};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        constexpr ConstIterator operator-(ConstIterator t_iter) const noexcept
        {
            return ConstIterator{m_ptr - static_cast<ptrdiff_t>(t_iter)};
        }

        /**
        * @brief
        *     Preincrement operator overload.
        */
        constexpr ConstIterator& operator++() noexcept
        {
            m_ptr++;
            return *this;
        }

        /**
        * @brief
        *     Postincrement operator overload.
        */
        constexpr ConstIterator operator++(int) noexcept
        {
            const ConstIterator copy{*this};
            ++*this;
            return copy;
        }

        /**
        * @brief
        *     Predecrement operator overload.
        */
        constexpr ConstIterator& operator--() noexcept
        {
            m_ptr--;
            return *this;
        }

        /**
        * @brief
        *     Postdecrement operator overload.
        */
        constexpr ConstIterator operator--(int) noexcept
        {
            const ConstIterator copy{*this};
            --*this;
            return copy;
        }

        operator uintptr_t() const noexcept;
        operator intptr_t() const noexcept;
    };
}

/**
* @brief
*     Cast operator overload.
*/
template<class T>
inline scan::ConstIterator<T>::operator uintptr_t() const noexcept
{
    return reinterpret_cast<uintptr_t>(m_ptr);
}

/**
* @brief
*     Cast operator overload.
*/
template<class T>
inline scan::ConstIterator<T>::operator intptr_t() const noexcept
{
    return reinterpret_cast<intptr_t>(m_ptr);
}

#endif // !SCAN_CONST_ITERATOR_H
