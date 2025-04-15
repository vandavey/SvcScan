/*
* @file
*     const_iterator.h
* @brief
*     Header file for a generic constant contiguous iterator.
*/
#pragma once

#ifndef SCAN_CONST_ITERATOR_H
#define SCAN_CONST_ITERATOR_H

#include <bit>
#include <cstdint>
#include "../concepts/concepts.h"
#include "../utils/aliases.h"
#include "iterator_traits.h"

namespace scan
{
    /**
    * @brief
    *     Generic constant contiguous iterator.
    */
    template<class T>
    class ConstIterator
    {
    public:  /* Type Aliases */
        using value_type      = IteratorTraits<const T>::value_type;
        using pointer         = IteratorTraits<const T>::pointer;
        using reference       = IteratorTraits<const T>::reference;
        using difference_type = IteratorTraits<const T>::difference_type;

        using iterator_category = IteratorTraits<const T>::iterator_category;
        using iterator_concept  = IteratorTraits<const T>::iterator_concept;

    private:  /* Fields */
        pointer m_ptr;  // Value type pointer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr ConstIterator() noexcept : m_ptr{nullptr}
        {
        }

        constexpr ConstIterator(const ConstIterator&) = default;
        constexpr ConstIterator(ConstIterator&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr explicit ConstIterator(pointer t_ptr) noexcept : m_ptr{t_ptr}
        {
        }

        virtual constexpr ~ConstIterator() = default;

    public:  /* Operators */
        constexpr ConstIterator& operator=(const ConstIterator&) = default;
        constexpr ConstIterator& operator=(ConstIterator&&) = default;

        /**
        * @brief
        *     Addition assignment operator overload.
        */
        constexpr ConstIterator& operator+=(Integral auto t_offset) noexcept
        {
            m_ptr += static_cast<intptr_t>(t_offset);
            return *this;
        }

        /**
        * @brief
        *     Subtraction assignment operator overload.
        */
        constexpr ConstIterator& operator-=(Integral auto t_offset) noexcept
        {
            m_ptr -= static_cast<intptr_t>(t_offset);
            return *this;
        }

        constexpr strong_ordering operator<=>(const ConstIterator&) const = default;

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
            return m_ptr[static_cast<ptrdiff_t>(t_index)];
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        constexpr ConstIterator operator+(Integral auto t_offset) const noexcept
        {
            return ConstIterator{m_ptr + static_cast<intptr_t>(t_offset)};
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        constexpr ConstIterator operator+(ConstIterator t_iter) const noexcept
        {
            return ConstIterator{m_ptr + t_iter.m_ptr};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        constexpr ConstIterator operator-(Integral auto t_offset) const noexcept
        {
            return ConstIterator{m_ptr - static_cast<intptr_t>(t_offset)};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        constexpr difference_type operator-(ConstIterator t_iter) const noexcept
        {
            return m_ptr - t_iter.m_ptr;
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

        /**
        * @brief
        *     Addition operator overload.
        */
        friend constexpr ConstIterator operator+(Integral auto t_offset,
                                                 const ConstIterator& t_iter)
            noexcept
        {
            return ConstIterator{t_iter.m_ptr + static_cast<intptr_t>(t_offset)};
        }
    };
}

#endif // !SCAN_CONST_ITERATOR_H
