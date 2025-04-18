/*
* @file
*     iterator.h
* @brief
*     Header file for a generic contiguous iterator.
*/
#pragma once

#ifndef SCAN_ITERATOR_H
#define SCAN_ITERATOR_H

#include <bit>
#include <cstdint>
#include "../concepts/concepts.h"
#include "../utils/aliases.h"
#include "iterator_traits.h"

namespace scan
{
    /**
    * @brief
    *     Generic contiguous iterator.
    */
    template<class T>
    class Iterator
    {
    public:  /* Type Aliases */
        using value_type      = IteratorTraits<T>::value_type;
        using pointer         = IteratorTraits<T>::pointer;
        using reference       = IteratorTraits<T>::reference;
        using difference_type = IteratorTraits<T>::difference_type;

        using iterator_category = IteratorTraits<T>::iterator_category;
        using iterator_concept  = IteratorTraits<T>::iterator_concept;

    private:  /* Fields */
        pointer m_ptr;  // Value type pointer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Iterator() noexcept : m_ptr{nullptr}
        {
        }

        constexpr Iterator(const Iterator&) = default;
        constexpr Iterator(Iterator&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr explicit Iterator(pointer t_ptr) noexcept : m_ptr{t_ptr}
        {
        }

        virtual constexpr ~Iterator() = default;

    public:  /* Operators */
        constexpr Iterator& operator=(const Iterator&) = default;
        constexpr Iterator& operator=(Iterator&&) = default;

        /**
        * @brief
        *     Addition assignment operator overload.
        */
        constexpr Iterator& operator+=(Integral auto t_offset) noexcept
        {
            m_ptr += static_cast<difference_type>(t_offset);
            return *this;
        }

        /**
        * @brief
        *     Subtraction assignment operator overload.
        */
        constexpr Iterator& operator-=(Integral auto t_offset) noexcept
        {
            m_ptr -= static_cast<difference_type>(t_offset);
            return *this;
        }

        constexpr strong_ordering operator<=>(const Iterator&) const = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr explicit operator uintptr_t() const noexcept
        {
            return std::bit_cast<uintptr_t>(m_ptr);
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr explicit operator intptr_t() const noexcept
        {
            return std::bit_cast<intptr_t>(m_ptr);
        }

        /**
        * @brief
        *     Dereference operator overload.
        */
        constexpr pointer operator->() const noexcept
        {
            return m_ptr;
        }

        /**
        * @brief
        *     Indirection operator overload.
        */
        constexpr reference operator*() const noexcept
        {
            return *m_ptr;
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr reference operator[](Integral auto t_index) const noexcept
        {
            return m_ptr[static_cast<difference_type>(t_index)];
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        constexpr Iterator operator+(Integral auto t_offset) const noexcept
        {
            return Iterator{m_ptr + static_cast<difference_type>(t_offset)};
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        constexpr Iterator operator+(Iterator t_iter) const noexcept
        {
            return Iterator{m_ptr + t_iter.m_ptr};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        constexpr Iterator operator-(Integral auto t_offset) const noexcept
        {
            return Iterator{m_ptr - static_cast<difference_type>(t_offset)};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        constexpr difference_type operator-(Iterator t_iter) const noexcept
        {
            return m_ptr - t_iter.m_ptr;
        }

        /**
        * @brief
        *     Preincrement operator overload.
        */
        constexpr Iterator& operator++() noexcept
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
            const Iterator copy{*this};
            ++*this;
            return copy;
        }

        /**
        * @brief
        *     Predecrement operator overload.
        */
        constexpr Iterator& operator--() noexcept
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
            const Iterator copy{*this};
            --*this;
            return copy;
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        friend constexpr Iterator operator+(Integral auto t_offset,
                                            const Iterator& t_iter)
            noexcept
        {
            return t_iter + t_offset;
        }
    };
}

#endif // !SCAN_ITERATOR_H
