/*
* @file
*     iterator.h
* @brief
*     Header file for a generic constant bidirectional iterator.
*/
#pragma once

#ifndef SCAN_ITERATOR_H
#define SCAN_ITERATOR_H

#include <compare>
#include <cstddef>
#include <iterator>
#include "../../concepts/concepts.h"

namespace scan
{
    /**
    * @brief
    *     Generic constant bidirectional iterator.
    */
    template<class T>
    class Iterator final
    {
    public:  /* Type Aliases */
        using value_type      = T;
        using pointer         = const value_type *;
        using reference       = const value_type &;
        using difference_type = ptrdiff_t;

        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept  = std::bidirectional_iterator_tag;

    private:  /* Fields */
        const value_type *m_ptr;  // Value type pointer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Iterator() noexcept
        {
            m_ptr = nullptr;
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Iterator(const Iterator &t_iter) noexcept
        {
            m_ptr = t_iter.m_ptr;
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Iterator(Iterator &&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Iterator(const value_type *t_ptr) noexcept
        {
            m_ptr = t_ptr;
        }

        constexpr virtual ~Iterator() = default;

    public:  /* Operators */
        constexpr Iterator &operator=(const Iterator &) = default;
        constexpr Iterator &operator=(Iterator &&) = default;

        /**
        * @brief
        *     Addition assignment operator overload.
        */
        template<Castable<ptrdiff_t> D>
        constexpr Iterator &operator+=(const D &t_offset) noexcept
        {
            m_ptr += static_cast<ptrdiff_t>(t_offset);
            return *this;
        }

        /**
        * @brief
        *     Subtraction assignment operator overload.
        */
        template<Castable<ptrdiff_t> D>
        constexpr Iterator &operator-=(const D &t_offset) noexcept
        {
            m_ptr -= static_cast<ptrdiff_t>(t_offset);
            return *this;
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator uintptr_t() const noexcept
        {
            return static_cast<uintptr_t>(m_ptr);
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator ptrdiff_t() const noexcept
        {
            return static_cast<ptrdiff_t>(operator uintptr_t());
        }

        /**
        * @brief
        *     Dereference operator overload.
        */
        constexpr const T *operator->() const noexcept
        {
            return m_ptr;
        }

        /**
        * @brief
        *     Indirection operator overload.
        */
        constexpr const T &operator*() const
        {
            return *m_ptr;
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr const T &operator[](const ptrdiff_t &t_idx) const
        {
            return m_ptr[t_idx];
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        template<Castable<ptrdiff_t> D>
        constexpr Iterator operator+(const D &t_offset) const noexcept
        {
            return m_ptr + static_cast<ptrdiff_t>(t_offset);
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        template<Castable<ptrdiff_t> D>
        constexpr Iterator operator-(const D &t_offset) const noexcept
        {
            return m_ptr - static_cast<ptrdiff_t>(t_offset);
        }

        /**
        * @brief
        *     Preincrement operator overload.
        */
        constexpr Iterator &operator++() noexcept
        {
            m_ptr++;
            return *this;
        }

        /**
        * @brief
        *     Postincrement operator overload.
        */
        constexpr Iterator operator++(int) noexcept
        {
            const Iterator copy{ *this };
            ++*this;
            return copy;
        }

        /**
        * @brief
        *     Predecrement operator overload.
        */
        constexpr Iterator &operator--() noexcept
        {
            m_ptr--;
            return *this;
        }

        /**
        * @brief
        *     Postdecrement operator overload.
        */
        constexpr Iterator operator--(int) noexcept
        {
            const Iterator copy{ *this };
            --*this;
            return copy;
        }

        constexpr std::strong_ordering operator<=>(const Iterator &) const = default;
    };
}

#endif // !SCAN_ITERATOR_H
