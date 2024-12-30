/*
* @file
*     list.h
* @brief
*     Header file for a generic container with an underlying vector.
*/
#pragma once

#ifndef SCAN_LIST_H
#define SCAN_LIST_H

#include <iterator>
#include <utility>
#include <vector>
#include "../concepts/concepts.h"
#include "../errors/arg_ex.h"
#include "../errors/logic_ex.h"
#include "../utils/aliases.h"
#include "../utils/const_defs.h"
#include "algo.h"
#include "const_iterator.h"
#include "iterator.h"

namespace scan
{
    /**
    * @brief
    *     Generic container that encapsulates a vector.
    */
    template<class T, class A = allocator<T>>
        requires Allocator<A, T>
    class List
    {
    public:  /* Type Aliases */
        using value_type      = T;
        using allocator_type  = A;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;

        using iterator               = Iterator<value_type>;
        using const_iterator         = ConstIterator<value_type>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:  /* Type Aliases */
        using vector_t = vector<value_type, A>;

    private:  /* Fields */
        vector_t m_buffer;  // Vector buffer

    public:  /* Constructors & Destructor */
        constexpr List() = default;
        constexpr List(const List&) = default;
        constexpr List(List&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        explicit constexpr List(size_t t_count, const A& t_alloc = {})
            : m_buffer(t_count, t_alloc)
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr List(const_iterator t_beg_iter,
                       const_iterator t_end_iter,
                       const A& t_alloc = {})
            : m_buffer(t_beg_iter, t_end_iter, t_alloc)
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr List(iterator t_beg_iter, iterator t_end_iter, const A& t_alloc = {})
            : m_buffer(t_beg_iter, t_end_iter, t_alloc)
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        template<Castable<T>... ArgsT>
            requires AtLeastOneParam<ArgsT...>
        constexpr List(const ArgsT&... t_args)
        {
            push_back(t_args...);
        }

        /**
        * @brief
        *     Initialize the object.
        */
        template<Castable<T>... ArgsT>
            requires AtLeastOneParam<ArgsT...>
        constexpr List(ArgsT&&... t_args)
        {
            (push_back(std::forward<ArgsT>(t_args)...));
        }

        /**
        * @brief
        *     Initialize the object.
        */
        template<Range R = vector_t>
            requires RangeValue<R, T>
        constexpr List(const R& t_range)
        {
            push_back(t_range);
        }

        /**
        * @brief
        *     Initialize the object.
        */
        template<Range R = vector_t>
            requires RangeValue<R, T>
        constexpr List(R&& t_range)
        {
            push_back(std::forward<R>(t_range));
        }

        virtual constexpr ~List() = default;

    public:  /* Operators */
        constexpr List& operator=(const List&) = default;
        constexpr List& operator=(List&&) = default;

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
        constexpr const value_type& operator[](ptrdiff_t t_index) const
        {
            if (!valid_index(t_index))
            {
                throw ArgEx{"t_index", "Index is out of the underlying vector bounds"};
            }
            return m_buffer.at(t_index >= 0 ? t_index : size() - algo::abs(t_index));
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr value_type& operator[](ptrdiff_t t_index)
        {
            if (!valid_index(t_index))
            {
                throw ArgEx{"t_index", "Index is out of the underlying vector bounds"};
            }
            return m_buffer.at(t_index >= 0 ? t_index : size() - algo::abs(t_index));
        }

    public:  /* Methods */
        /**
        * @brief
        *     Add the given value to the underlying vector.
        */
        template<Castable<T> V>
        constexpr void push_back(const V& t_value)
        {
            m_buffer.push_back(t_value);
        }

        /**
        * @brief
        *     Add the given value to the underlying vector.
        */
        template<Castable<T> V>
        constexpr void push_back(V&& t_value)
        {
            m_buffer.push_back(std::forward<value_type>(t_value));
        }

        /**
        * @brief
        *     Add the given values to the underlying vector.
        */
        template<Castable<T>... ArgsT>
            requires AtLeastOneParam<ArgsT...>
        constexpr void push_back(const ArgsT&... t_args)
        {
            (push_back(t_args), ...);
        }

        /**
        * @brief
        *     Add the given values to the underlying vector.
        */
        template<Castable<T>... ArgsT>
            requires AtLeastOneParam<ArgsT...>
        constexpr void push_back(ArgsT&&... t_args)
        {
            (push_back(std::forward<ArgsT>(t_args)), ...);
        }

        /**
        * @brief
        *     Add the given range of values to the underlying vector.
        */
        template<Range R>
            requires RangeValue<R, T>
        constexpr void push_back(const R& t_range)
        {
            for (const value_type& value : t_range)
            {
                push_back(value);
            }
        }

        /**
        * @brief
        *     Add the given range of values to the underlying vector.
        */
        template<Range R>
            requires RangeValue<R, T>
        constexpr void push_back(R&& t_range)
        {
            RangeIterator auto it{t_range.begin()};

            for (size_t i{0_st}; i < t_range.size(); ++i, ++it)
            {
                push_back(std::forward<value_type>((*it)));
            }
        }

        /**
        * @brief
        *     Remove all values from the underlying vector.
        */
        constexpr void clear() noexcept
        {
            m_buffer.clear();
        }

        /**
        * @brief
        *     Remove the first matching value from the underlying vector.
        */
        constexpr void remove(const value_type& t_value)
        {
            const size_t offset{find(t_value)};

            if (algo::is_npos(offset))
            {
                throw ArgEx{"t_value", "No matching value found to remove"};
            }

            m_buffer.erase(m_buffer.begin() + offset);
            shrink_to_fit();
        }

        /**
        * @brief
        *     Remove the underlying vector value at the given index.
        */
        constexpr void remove_at(size_t t_index)
        {
            if (!valid_index(t_index))
            {
                throw ArgEx{"t_index", "Index is out of the underlying vector bounds"};
            }

            m_buffer.erase(m_buffer.begin() + t_index);
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
        *     Determine whether the underlying vector contains any of the given values.
        */
        template<Castable<T>... ArgsT>
        constexpr bool any(const ArgsT&... t_args) const
        {
            return (contains(t_args) || ...);
        }

        /**
        * @brief
        *     Determine whether the underlying vector contains any of the given values.
        */
        template<Castable<T>... ArgsT>
        constexpr bool any(ArgsT&&... t_args) const
        {
            return (contains(std::forward<ArgsT>(t_args)) || ...);
        }

        /**
        * @brief
        *     Determine whether the underlying vector contains the given value.
        */
        constexpr bool contains(const value_type& t_value) const
        {
            return algo::contains(*this, t_value);
        }

        /**
        * @brief
        *     Determine whether the underlying vector contains the given value.
        */
        constexpr bool contains(value_type&& t_value) const
        {
            return algo::contains(*this, std::forward<value_type>(t_value));
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
        constexpr bool valid_index(ptrdiff_t t_index) const
        {
            const ptrdiff_t count{static_cast<ptrdiff_t>(size())};
            return t_index >= 0 ? t_index < count : algo::abs(t_index) <= count;
        }

        /**
        * @brief
        *     Find the index of the first matching value in the underlying vector.
        */
        constexpr size_t find(const value_type& t_value) const
        {
            const_iterator iter{algo::find(*this, t_value)};
            return iter == cend() ? NPOS : ranges::distance(cbegin(), iter);
        }

        /**
        * @brief
        *     Find the index of the first matching value in the underlying vector.
        */
        constexpr size_t find(value_type&& t_value) const
        {
            const_iterator iter{algo::find(*this, std::forward<value_type>(t_value))};
            return iter == cend() ? NPOS : ranges::distance(cbegin(), iter);
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
        constexpr const value_type* data() const noexcept
        {
            return m_buffer.data();
        }

        /**
        * @brief
        *     Get a pointer to the array of the underlying vector.
        */
        constexpr value_type* data() noexcept
        {
            return m_buffer.data();
        }

        /**
        * @brief
        *     Get a constant iterator to the first value of the underlying vector.
        */
        constexpr const_iterator begin() const noexcept
        {
            return cbegin();
        }

        /**
        * @brief
        *     Get a constant iterator to the first value of the underlying vector.
        */
        constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator{data()};
        }

        /**
        * @brief
        *     Get a constant iterator to the past-the-end value of the underlying vector.
        */
        constexpr const_iterator cend() const noexcept
        {
            return const_iterator{begin() + size()};
        }

        /**
        * @brief
        *     Get a constant iterator to the past-the-end value of the underlying vector.
        */
        constexpr const_iterator end() const noexcept
        {
            return cend();
        }

        /**
        * @brief
        *     Get an iterator to the first value of the underlying vector.
        */
        constexpr iterator begin() noexcept
        {
            return iterator{data()};
        }

        /**
        * @brief
        *     Get an iterator to the past-the-end value of the underlying vector.
        */
        constexpr iterator end() noexcept
        {
            return iterator{begin() + size()};
        }

        /**
        * @brief
        *     Join the underlying values using the given delimiter.
        */
        constexpr string join(const string& t_delim) const requires LShift<T>
        {
            return algo::join(*this, t_delim);
        }

        /**
        * @brief
        *     Join the underlying values using a line-feed delimiter.
        */
        constexpr string join_lines() const requires LShift<T>
        {
            return algo::join_lines(*this);
        }

        /**
        * @brief
        *     Get a constant reference to the value located at the given vector index.
        */
        constexpr const value_type& at(ptrdiff_t t_index) const
        {
            return (*this)[t_index];
        }

        /**
        * @brief
        *     Get a reference to the value located at the given vector index.
        */
        constexpr value_type& at(ptrdiff_t t_index)
        {
            return (*this)[t_index];
        }

        /**
        * @brief
        *     Get a constant reference to the last value in the underlying vector.
        */
        constexpr const value_type& last() const
        {
            if (empty())
            {
                throw LogicEx{"List<T>::last", "Underlying vector is empty"};
            }
            return (*this)[-1];
        }

        /**
        * @brief
        *     Get a reference to the last value in the underlying vector.
        */
        constexpr value_type& last()
        {
            if (empty())
            {
                throw LogicEx{"List<T>::last", "Underlying vector is empty"};
            }
            return (*this)[-1];
        }

        /**
        * @brief
        *     Get a constant reference to the underlying vector.
        */
        constexpr const vector_t& vector() const noexcept
        {
            return m_buffer;
        }

        /**
        * @brief
        *     Get a reference to the underlying vector.
        */
        constexpr vector_t& vector() noexcept
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
        *     Retrieve a subrange of the underlying values
        *     based on the given start and end iterators.
        */
        constexpr List slice(iterator t_beg_iter, iterator t_end_iter) const
        {
            if (!valid_iterator(t_beg_iter))
            {
                throw ArgEx{"t_beg_iter", "Invalid iterator received"};
            }

            if (!valid_iterator(t_end_iter))
            {
                throw ArgEx{"t_end_iter", "Invalid iterator received"};
            }

            if (t_beg_iter > t_end_iter)
            {
                throw ArgEx{{"t_beg_iter", "t_end_iter"}, "Invalid iterator(s) received"};
            }

            return List{t_beg_iter, t_end_iter};
        }

        /**
        * @brief
        *     Retrieve a subrange of the underlying values
        *     based on the given start and end iterators.
        */
        constexpr List slice(const_iterator t_beg_iter, const_iterator t_end_iter) const
        {
            if (!valid_iterator(t_beg_iter))
            {
                throw ArgEx{"t_beg_iter", "Invalid iterator received"};
            }

            if (!valid_iterator(t_end_iter))
            {
                throw ArgEx{"t_end_iter", "Invalid iterator received"};
            }

            if (t_beg_iter > t_end_iter)
            {
                throw ArgEx{{"t_beg_iter", "t_end_iter"}, "Invalid iterator(s) received"};
            }

            return List{t_beg_iter, t_end_iter};
        }

        /**
        * @brief
        *     Retrieve a subrange of the underlying values
        *     based on the given start and end list indexes.
        */
        constexpr List slice(size_t t_beg_index, size_t t_end_index = NPOS) const
        {
            bool index_invalid{algo::is_npos(t_end_index)};
            const_iterator end_iter{index_invalid ? cend() : cbegin() + t_end_index};

            return slice(const_iterator{cbegin() + t_beg_index}, end_iter);
        }

    private:  /* Methods */
        /**
        * @brief
        *     Determine whether the given iterator is a
        *     valid iterator of the underlying vector.
        */
        constexpr bool valid_iterator(iterator t_iter) const
        {
            return t_iter >= begin() && t_iter <= end();
        }

        /**
        * @brief
        *     Determine whether the given iterator is a
        *     valid iterator of the underlying vector.
        */
        constexpr bool valid_iterator(const_iterator t_iter) const
        {
            return t_iter >= cbegin() && t_iter <= cend();
        }
    };
}

#endif // !SCAN_LIST_H
