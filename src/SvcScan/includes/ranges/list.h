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
#include <memory>
#include <utility>
#include <vector>
#include "../concepts/concepts.h"
#include "../errors/arg_ex.h"
#include "../errors/error_const_defs.h"
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
    template<class T, Allocator<T> AllocT = allocator<T>>
    class List
    {
    public:  /* Type Aliases */
        using value_type      = T;
        using allocator_type  = AllocT;
        using pointer         = allocator_traits<allocator_type>::pointer;
        using const_pointer   = allocator_traits<allocator_type>::const_pointer;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;

        using iterator               = Iterator<value_type>;
        using const_iterator         = ConstIterator<value_type>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:  /* Type Aliases */
        using vector_t = vector<value_type, allocator_type>;

    private:  /* Fields */
        vector_t m_vect;  // Data vector

    public:  /* Constructors & Destructor */
        constexpr List() = default;
        constexpr List(const List&) = default;
        constexpr List(List&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr explicit List(size_type t_count, const allocator_type& t_alloc = {})
            : m_vect(t_count, t_alloc)
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr List(const_iterator t_beg_iter,
                       const_iterator t_end_iter,
                       const allocator_type& t_alloc = {})
            : m_vect(t_beg_iter, t_end_iter, t_alloc)
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr List(iterator t_beg_iter,
                       iterator t_end_iter,
                       const allocator_type& t_alloc = {})
            : m_vect(t_beg_iter, t_end_iter, t_alloc)
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr List(initializer_list<value_type> t_init_list,
                       const allocator_type& t_alloc = {})
            : m_vect(t_init_list, t_alloc)
        {
        }

        /**
        * @brief
        *     Initialize the object.
        */
        template<RangeOf<T> R>
        constexpr List(R&& t_range, const allocator_type& t_alloc = {}) : m_vect(t_alloc)
        {
            push_back(std::forward<R>(t_range));
        }

        virtual constexpr ~List() = default;

    public:  /* Operators */
        constexpr List& operator=(const List&) = default;
        constexpr List& operator=(List&&) = default;

        constexpr strong_ordering operator<=>(const List&) const = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator vector_t() const noexcept
        {
            return m_vect;
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr const_reference operator[](Integral auto t_index) const
        {
            return at(t_index);
        }

        /**
        * @brief
        *     Subscript operator overload.
        */
        constexpr reference operator[](Integral auto t_index)
        {
            return at(t_index);
        }

    public:  /* Methods */
        /**
        * @brief
        *     Append the given value to the underlying vector.
        */
        template<Castable<T> T2>
        constexpr void push_back(T2&& t_value)
        {
            m_vect.push_back(std::forward<T2>(t_value));
        }

        /**
        * @brief
        *     Append the given range of values to the underlying vector.
        */
        template<RangeOf<T> R>
        constexpr void push_back(R&& t_range)
        {
            algo::transfer(std::forward<R>(t_range), *this);
        }

        /**
        * @brief
        *     Remove all values from the underlying vector.
        */
        constexpr void clear() noexcept
        {
            m_vect.clear();
        }

        /**
        * @brief
        *     Remove the underlying vector value at the given index.
        */
        constexpr void remove_at(size_type t_index)
        {
            if (!valid_index(t_index))
            {
                throw ArgEx{INVALID_VECTOR_INDEX_MSG, "t_index"};
            }

            m_vect.erase(m_vect.begin() + t_index);
            shrink_to_fit();
        }

        /**
        * @brief
        *     Request that unused capacity be freed from the underlying vector.
        */
        constexpr void shrink_to_fit()
        {
            m_vect.shrink_to_fit();
        }

        /**
        * @brief
        *     Determine whether the underlying vector contains any of the given values.
        */
        template<Castable<T>... ArgsT>
            requires AtLeastOne<ArgsT...>
        constexpr bool any(ArgsT&&... t_args) const
        {
            return (contains(std::forward<ArgsT>(t_args)) || ...);
        }

        /**
        * @brief
        *     Determine whether the underlying vector contains the given value.
        */
        template<Castable<T> T2>
        constexpr bool contains(T2&& t_value) const
        {
            return algo::contains(*this, value_type{std::forward<T2>(t_value)});
        }

        /**
        * @brief
        *     Determine whether the underlying vector is empty.
        */
        constexpr bool empty() const noexcept
        {
            return m_vect.empty();
        }

        /**
        * @brief
        *     Determine whether the given index is a valid index of the underlying vector.
        */
        constexpr bool valid_index(Integral auto t_index) const noexcept
        {
            return resolve_index(t_index) < size();
        }

        /**
        * @brief
        *     Get the current size of the underlying vector.
        */
        constexpr size_type size() const noexcept
        {
            return static_cast<size_type>(m_vect.size());
        }

        /**
        * @brief
        *     Get a constant pointer to the array of the underlying vector.
        */
        constexpr const_pointer data() const noexcept
        {
            return m_vect.data();
        }

        /**
        * @brief
        *     Get a pointer to the array of the underlying vector.
        */
        constexpr pointer data() noexcept
        {
            return m_vect.data();
        }

        /**
        * @brief
        *     Get a constant iterator to the beginning of the underlying vector.
        */
        constexpr const_iterator begin() const noexcept
        {
            return cbegin();
        }

        /**
        * @brief
        *     Get a constant iterator to the beginning of the underlying vector.
        */
        constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator{data()};
        }

        /**
        * @brief
        *     Get a constant iterator to the end of the underlying vector.
        */
        constexpr const_iterator cend() const noexcept
        {
            return const_iterator{begin() + size()};
        }

        /**
        * @brief
        *     Get a constant iterator to the end of the underlying vector.
        */
        constexpr const_iterator end() const noexcept
        {
            return cend();
        }

        /**
        * @brief
        *     Get an iterator to the beginning of the underlying vector.
        */
        constexpr iterator begin() noexcept
        {
            return iterator{data()};
        }

        /**
        * @brief
        *     Get an iterator to the end of the underlying vector.
        */
        constexpr iterator end() noexcept
        {
            return iterator{begin() + size()};
        }

        /**
        * @brief
        *     Get a constant reference to the value located at the given vector index.
        */
        constexpr const_reference at(Integral auto t_index) const
        {
            if (!valid_index(t_index))
            {
                throw ArgEx{INVALID_VECTOR_INDEX_MSG, "t_index"};
            }
            return m_vect.at(resolve_index(t_index));
        }

        /**
        * @brief
        *     Get a constant reference to the last value in the underlying vector.
        */
        constexpr const_reference last() const
        {
            if (empty())
            {
                throw LogicEx{EMPTY_VECTOR_MSG, "List::last"};
            }
            return (*this)[-1];
        }

        /**
        * @brief
        *     Get a reference to the value located at the given vector index.
        */
        constexpr reference at(Integral auto t_index)
        {
            if (!valid_index(t_index))
            {
                throw ArgEx{INVALID_VECTOR_INDEX_MSG, "t_index"};
            }
            return m_vect.at(resolve_index(t_index));
        }

        /**
        * @brief
        *     Append the given values to the underlying vector.
        */
        template<Constructible<T>... ArgsT>
        constexpr reference emplace_back(ArgsT&&... t_args)
        {
            return m_vect.emplace_back(std::forward<ArgsT>(t_args)...);
        }

        /**
        * @brief
        *     Get a reference to the last value in the underlying vector.
        */
        constexpr reference last()
        {
            if (empty())
            {
                throw LogicEx{EMPTY_VECTOR_MSG, "List::last"};
            }
            return (*this)[-1];
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
                throw ArgEx{INVALID_ITER_MSG, "t_beg_iter"};
            }

            if (!valid_iterator(t_end_iter))
            {
                throw ArgEx{INVALID_ITER_MSG, "t_end_iter"};
            }

            if (t_beg_iter > t_end_iter)
            {
                throw ArgEx{INVALID_ITER_COMBO_MSG, "t_beg_iter", "t_end_iter"};
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
                throw ArgEx{INVALID_ITER_MSG, "t_beg_iter"};
            }

            if (!valid_iterator(t_end_iter))
            {
                throw ArgEx{INVALID_ITER_MSG, "t_end_iter"};
            }

            if (t_beg_iter > t_end_iter)
            {
                throw ArgEx{INVALID_ITER_COMBO_MSG, "t_beg_iter", "t_end_iter"};
            }

            return List{t_beg_iter, t_end_iter};
        }

        /**
        * @brief
        *     Retrieve a subrange of the underlying values
        *     based on the given start and end list indexes.
        */
        constexpr List slice(size_type t_beg_index, size_type t_end_index = NPOS) const
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

        /**
        * @brief
        *     Resolve the given index to adjust for negative range indexing.
        */
        constexpr size_type resolve_index(Integral auto t_index) const noexcept
        {
            return size_type{t_index < 0 ? size() - algo::abs(t_index) : t_index};
        }
    };
}

#endif // !SCAN_LIST_H
