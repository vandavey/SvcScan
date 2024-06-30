/*
* @file
*     list.h
* @brief
*     Header file for a generic container with an underlying vector.
*/
#pragma once

#ifndef SCAN_LIST_H
#define SCAN_LIST_H

#include <concepts>
#include <vector>
#include "../../concepts/concepts.h"
#include "../../errors/arg_ex.h"
#include "../../errors/logic_ex.h"
#include "../../utils/algo.h"
#include "../../utils/alias.h"
#include "../../utils/const_defs.h"
#include "iterator.h"

namespace scan
{
    /**
    * @brief
    *     Generic container that encapsulates a vector.
    */
    template<class T>
    class List
    {
    public:  /* Type Aliases */
        using value_type      = T;
        using pointer         = value_type *;
        using const_pointer   = const value_type *;
        using reference       = value_type &;
        using const_reference = const value_type &;
        using difference_type = std::ptrdiff_t;

        using iterator       = Iterator<value_type>;
        using const_iterator = iterator;

    private:  /* Type Aliases */
        using vector_t = vector<value_type>;

    private:  /* Constants */
        static constexpr size_t NPOS = static_cast<size_t>(-1);  // Max collection size

    private:  /* Fields */
        vector_t m_buffer;  // Vector buffer

    public:  /* Constructors & Destructor */
        List() = default;
        List(const List &) = default;
        List(List &&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        template<Range R>
        constexpr List(const R &t_range)
        {
            add_range(t_range);
        }

        /**
        * @brief
        *     Initialize the object.
        */
        template<Castable<T> ...ArgsT>
        constexpr List(const ArgsT &...t_args) requires(sizeof...(t_args) > 0)
        {
            add(t_args...);
        }

        virtual ~List() = default;

    public:  /* Operators */
        List &operator=(const List &) = default;
        List &operator=(List &&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator vector_t() const noexcept
        {
            return m_buffer;
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr const T &operator[](const ptrdiff_t &t_idx) const
        {
            if (!valid_index(t_idx))
            {
                throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
            }
            return m_buffer.at(t_idx >= 0 ? t_idx : size() - algo::abs(t_idx));
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr T &operator[](const ptrdiff_t &t_idx)
        {
            if (!valid_index(t_idx))
            {
                throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
            }
            return m_buffer.at(t_idx >= 0 ? t_idx : size() - algo::abs(t_idx));
        }

    public:  /* Methods */
        /**
        * @brief
        *     Create a list that contains all integers within the given range bounds.
        */
        static constexpr List fill(const T &t_min, const T &t_max)
            requires std::integral<T>
        {
            if (t_min >= t_max)
            {
                throw ArgEx{ "t_max", "Maximum must be greater than minimum" };
            }
            List list;

            for (value_type i{ t_min }; i <= t_max; i++)
            {
                list.add(i);
            }
            return list;
        }

        /**
        * @brief
        *     Add the given element to the underlying vector.
        */
        constexpr void add(const value_type &t_elem)
        {
            m_buffer.push_back(t_elem);
        }

        /**
        * @brief
        *     Add the given elements to the underlying vector.
        */
        template<Castable<T> ...ArgsT>
        constexpr void add(const ArgsT &...t_args) requires(sizeof...(t_args) > 0)
        {
            (m_buffer.push_back(t_args), ...);
        }

        /**
        * @brief
        *     Add the given range of elements to the underlying vector.
        */
        template<Range R>
        constexpr void add_range(const R &t_range)
        {
            for (const value_type &elem : t_range)
            {
                add(elem);
            }
        }

        /**
        * @brief
        *     Remove all elements from the underlying vector.
        */
        constexpr void clear() noexcept
        {
            m_buffer.clear();
        }

        /**
        * @brief
        *     Remove the first matching element in the underlying vector.
        */
        constexpr void remove(const value_type &t_elem)
        {
            const size_t offset{ find(t_elem) };

            if (offset == NPOS)
            {
                throw ArgEx{ "t_elem", "No matching element found to remove" };
            }

            m_buffer.erase(m_buffer.begin() + offset);
            shrink_to_fit();
        }

        /**
        * @brief
        *     Remove the underlying vector element at the given index.
        */
        constexpr void remove_at(const size_t &t_offset)
        {
            if (t_offset >= size())
            {
                throw ArgEx{ "t_offset", "Index is out of the underlying vector bounds" };
            }

            m_buffer.erase(m_buffer.begin() + t_offset);
            shrink_to_fit();
        }

        /**
        * @brief
        *     Request that unused capacity be freed from the underlying vector.
        */
        constexpr void shrink_to_fit()
        {
            m_buffer.shrink_to_fit();
        }

        /**
        * @brief
        *     Determine whether the underlying vector contains any of the given elements.
        */
        template<Castable<T> ...ArgsT>
        constexpr bool any(const ArgsT &...t_args) const
        {
            return (contains(t_args) || ...);
        }

        /**
        * @brief
        *     Determine whether the underlying vector contains the given element.
        */
        constexpr bool contains(const value_type &t_elem) const
        {
            return find(t_elem) != NPOS;
        }

        /**
        * @brief
        *     Determine whether the underlying vector is empty.
        */
        constexpr bool empty() const noexcept
        {
            return m_buffer.empty();
        }

        /**
        * @brief
        *     Determine whether the given index is a valid index of the underlying vector.
        */
        constexpr bool valid_index(const ptrdiff_t &t_idx) const
        {
            ptrdiff_t count{ static_cast<ptrdiff_t>(size()) };
            return t_idx >= 0 ? t_idx < count : algo::abs(t_idx) <= count;
        }

        /**
        * @brief
        *     Find the index of the first matching element in the underlying vector.
        */
        constexpr size_t find(const value_type &t_elem) const
        {
            const iterator iter{ ranges::find(*this, t_elem) };
            return iter == end() ? NPOS : ranges::distance(begin(), iter);
        }

        /**
        * @brief
        *     Get the current size of the underlying vector.
        */
        constexpr size_t size() const noexcept
        {
            return m_buffer.size();
        }

        /**
        * @brief
        *     Get a constant pointer to the array of the underlying vector.
        */
        constexpr const value_type *data() const noexcept
        {
            return m_buffer.data();
        }

        /**
        * @brief
        *     Get a pointer to the array of the underlying vector.
        */
        constexpr value_type *data() noexcept
        {
            return m_buffer.data();
        }

        /**
        * @brief
        *     Get a constant iterator to the first element in the underlying vector.
        */
        constexpr iterator begin() const noexcept
        {
            return data();
        }

        /**
        * @brief
        *     Get a constant iterator to the past-the-end element in the underlying vector. 
        */
        constexpr iterator end() const noexcept
        {
            return begin() + size();
        }

        /**
        * @brief
        *     Join the underlying elements using the given delimiter.
        */
        constexpr string join(const string &t_delim) const requires LShift<T>
        {
            return algo::join(*this, t_delim);
        }

        /**
        * @brief
        *     Join the underlying elements using a line feed delimiter.
        */
        constexpr string join_lines() const requires LShift<T>
        {
            return join(LF);
        }

        /**
        * @brief
        *     Get a constant reference to the element located at the given vector index.
        */
        constexpr const T &at(const ptrdiff_t &t_idx) const
        {
            return (*this)[t_idx];
        }

        /**
        * @brief
        *     Get a reference to the element located at the given vector index.
        */
        constexpr T &at(const ptrdiff_t &t_idx)
        {
            return (*this)[t_idx];
        }

        /**
        * @brief
        *     Get a constant reference to the last element in the underlying vector.
        */
        constexpr const T &last() const
        {
            if (empty())
            {
                throw LogicEx{ "List<T>::last", "Underlying vector is empty" };
            }
            return (*this)[-1];
        }

        /**
        * @brief
        *     Get a reference to the last element in the underlying vector.
        */
        constexpr T &last()
        {
            if (empty())
            {
                throw LogicEx{ "List<T>::last", "Underlying vector is empty" };
            }
            return (*this)[-1];
        }

        /**
        * @brief
        *     Get a constant reference to the underlying vector.
        */
        constexpr const vector_t &vector() const noexcept
        {
            return m_buffer;
        }

        /**
        * @brief
        *     Get a reference to the underlying vector.
        */
        constexpr vector_t &vector() noexcept
        {
            return m_buffer;
        }

        /**
        * @brief
        *     Make a copy of the current list object.
        */
        constexpr List copy() const noexcept
        {
            return *this;
        }

        /**
        * @brief
        *     Retrieve a subrange of the underlying elements
        *     based on the given start and end list iterators.
        */
        constexpr List slice(const iterator &t_beg, const iterator &t_end) const
        {
            if (!valid_iterator(t_beg))
            {
                throw ArgEx{ "t_beg", "Invalid iterator received" };
            }

            if (!valid_iterator(t_end))
            {
                throw ArgEx{ "t_end", "Invalid iterator received" };
            }

            if (t_beg > t_end)
            {
                throw ArgEx{ { "t_beg", "t_end" }, "Invalid iterator received" };
            }
            List list;

            for (iterator it{ t_beg }; it != t_end; ++it)
            {
                list.add(*it);
            }
            return list;
        }

        /**
        * @brief
        *     Retrieve a subrange of the underlying elements
        *     based on the given start and end list indexes.
        */
        constexpr List slice(const size_t &t_beg_idx, const size_t &t_end_idx = NPOS)
            const
        {
            return slice(begin() + t_beg_idx, t_end_idx == NPOS ? end() : begin() + t_end_idx);
        }

    private:  /* Methods */
        /**
        * @brief
        *     Determine whether the given iterator is a valid iterator of the underlying vector.
        */
        constexpr bool valid_iterator(const iterator &t_iter) const
        {
            return t_iter >= begin() && t_iter <= end();
        }
    };
}

#endif // !SCAN_LIST_H
