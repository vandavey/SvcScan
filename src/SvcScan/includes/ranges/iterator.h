/*
* @file
*     iterator.h
* @brief
*     Header file for a generic bidirectional iterator.
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
    *     Generic bidirectional iterator.
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
        value_type* m_ptr;  // Value type pointer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Iterator() noexcept : Iterator{nullptr}
        {
        }

        constexpr Iterator(const Iterator&) = default;
        constexpr Iterator(Iterator&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Iterator(value_type* t_ptr) noexcept
        {
            m_ptr = t_ptr;
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
            m_ptr += static_cast<intptr_t>(t_offset);
            return *this;
        }

        /**
        * @brief
        *     Subtraction assignment operator overload.
        */
        constexpr Iterator& operator-=(Integral auto t_offset) noexcept
        {
            m_ptr -= static_cast<intptr_t>(t_offset);
            return *this;
        }

        constexpr strong_ordering operator<=>(const Iterator&) const = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator uintptr_t() const noexcept
        {
            return std::bit_cast<uintptr_t>(m_ptr);
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator intptr_t() const noexcept
        {
            return std::bit_cast<intptr_t>(m_ptr);
        }

        /**
        * @brief
        *     Dereference operator overload.
        */
        constexpr value_type* operator->() const noexcept
        {
            return m_ptr;
        }

        /**
        * @brief
        *     Indirection operator overload.
        */
        constexpr value_type& operator*() const noexcept
        {
            return *m_ptr;
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr value_type& operator[](Integral auto t_index) const noexcept
        {
            return m_ptr[static_cast<ptrdiff_t>(t_index)];
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        constexpr Iterator operator+(Integral auto t_offset) const noexcept
        {
            return Iterator{m_ptr + static_cast<intptr_t>(t_offset)};
        }

        /**
        * @brief
        *     Addition operator overload.
        */
        constexpr Iterator operator+(Iterator t_iter) const noexcept
        {
            return Iterator{m_ptr + static_cast<intptr_t>(t_iter)};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        constexpr Iterator operator-(Integral auto t_offset) const noexcept
        {
            return Iterator{m_ptr - static_cast<intptr_t>(t_offset)};
        }

        /**
        * @brief
        *     Subtraction operator overload.
        */
        constexpr Iterator operator-(Iterator t_iter) const noexcept
        {
            return Iterator{m_ptr - static_cast<intptr_t>(t_iter)};
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
    };
}

#endif // !SCAN_ITERATOR_H
