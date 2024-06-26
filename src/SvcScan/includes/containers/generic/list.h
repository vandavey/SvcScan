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
#include <cstdlib>
#include <string>
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
        List(const List &t_list) noexcept;
        List(List &&) = default;

        template<Range R>
        List(const R &t_range);

        template<Castable<T> ...ArgsT>
        List(const ArgsT &...t_args) requires(sizeof...(t_args) > 0);

        virtual ~List() = default;

    public:  /* Operators */
        List &operator=(const List &t_list) noexcept;
        List &operator=(List &&) = default;

        operator vector_t() const noexcept;

        T &operator[](const ptrdiff_t &t_idx);
        const T &operator[](const ptrdiff_t &t_idx) const;

    public:  /* Methods */
        static List fill(const T &t_min, const T &t_max) requires std::integral<T>;

        void add(const value_type &t_elem);

        template<Castable<T> ...ArgsT>
        void add(const ArgsT &...t_args) requires(sizeof...(t_args) > 0);

        template<Range R>
        void add_range(const R &t_range);

        void clear() noexcept;
        void remove(const value_type &t_elem);
        void remove_at(const size_t &t_offset);
        void shrink_to_fit();

        template<Castable<T> ...ArgsT>
        bool any(const ArgsT &...t_args) const;

        bool contains(const value_type &t_elem) const;
        bool empty() const noexcept;
        bool valid_index(const ptrdiff_t &t_idx) const;

        size_t find(const value_type &t_elem) const;
        size_t size() const noexcept;

        const value_type *data() const noexcept;
        value_type *data() noexcept;

        iterator begin() const noexcept;
        iterator end() const noexcept;

        string join(const string &t_delim) const requires LShift<T>;
        string join_lines() const requires LShift<T>;

        const T &at(const ptrdiff_t &t_idx) const;
        T &at(const ptrdiff_t &t_idx);
        const T &last() const;
        T &last();

        const vector_t &vector() const noexcept;
        vector_t &vector() noexcept;

        List copy() const noexcept;
        List slice(const iterator &t_beg, const iterator &t_end) const;
        List slice(const size_t &t_beg_idx, const size_t &t_end_idx = NPOS) const;

    private:  /* Methods */
        bool valid_iterator(const iterator &t_iter) const;
    };
}

/**
* @brief
*     Initialize the object.
*/
template<class T>
inline scan::List<T>::List(const List &t_list) noexcept
{
    *this = t_list;
}

/**
* @brief
*     Initialize the object.
*/
template<class T>
template<scan::Range R>
inline scan::List<T>::List(const R &t_range)
{
    add_range(t_range);
}

/**
* @brief
*     Initialize the object.
*/
template<class T>
template<scan::Castable<T> ...ArgsT>
inline scan::List<T>::List(const ArgsT &...t_args) requires(sizeof...(t_args) > 0)
{
    add(t_args...);
}

/**
* @brief
*     Copy assignment operator overload.
*/
template<class T>
inline scan::List<T> &scan::List<T>::operator=(const List &t_list) noexcept
{
    m_buffer = t_list.m_buffer;
    return *this;
}

/**
* @brief
*     Cast operator overload.
*/
template<class T>
inline scan::List<T>::operator vector_t() const noexcept
{
    return m_buffer;
}

/**
* @brief
*     Subscript operator overload.
*/
template<class T>
inline T &scan::List<T>::operator[](const ptrdiff_t &t_idx)
{
    if (!valid_index(t_idx))
    {
        throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
    }
    return m_buffer.at(t_idx >= 0 ? t_idx : size() - std::abs(t_idx));
}

/**
* @brief
*     Subscript operator overload.
*/
template<class T>
inline const T &scan::List<T>::operator[](const ptrdiff_t &t_idx) const
{
    if (!valid_index(t_idx))
    {
        throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
    }
    return m_buffer.at(t_idx >= 0 ? t_idx : size() - std::abs(t_idx));
}

/**
* @brief
*     Create a list that contains all integers within the given range bounds.
*/
template<class T>
inline scan::List<T> scan::List<T>::fill(const T &t_min, const T &t_max)
    requires std::integral<T>
{
    if (t_min >= t_max)
    {
        throw ArgEx{ { "t_min", "t_max" }, "Maximum must be greater than minimum" };
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
template<class T>
inline void scan::List<T>::add(const value_type &t_elem)
{
    m_buffer.push_back(t_elem);
}

/**
* @brief
*     Add the given elements to the underlying vector.
*/
template<class T>
template<scan::Castable<T> ...ArgsT>
inline void scan::List<T>::add(const ArgsT &...t_args) requires(sizeof...(t_args) > 0)
{
    (m_buffer.push_back(t_args), ...);
}

/**
* @brief
*     Add the given range of elements to the underlying vector.
*/
template<class T>
template<scan::Range R>
inline void scan::List<T>::add_range(const R &t_range)
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
template<class T>
inline void scan::List<T>::clear() noexcept
{
    m_buffer.clear();
}

/**
* @brief
*     Remove the first matching element in the underlying vector.
*/
template<class T>
inline void scan::List<T>::remove(const value_type &t_elem)
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
template<class T>
inline void scan::List<T>::remove_at(const size_t &t_offset)
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
*     Request that unused capacity memory in the underlying vector be released.
*/
template<class T>
inline void scan::List<T>::shrink_to_fit()
{
    m_buffer.shrink_to_fit();
}

/**
* @brief
*     Determine whether the underlying vector contains any of the given elements.
*/
template<class T>
template<scan::Castable<T> ...ArgsT>
inline bool scan::List<T>::any(const ArgsT &...t_args) const
{
    return (contains(t_args) || ...);
}

/**
* @brief
*     Determine whether the underlying vector contains the given element.
*/
template<class T>
inline bool scan::List<T>::contains(const value_type &t_elem) const
{
    return find(t_elem) != NPOS;
}

/**
* @brief
*     Determine whether the underlying vector is empty.
*/
template<class T>
inline bool scan::List<T>::empty() const noexcept
{
    return m_buffer.empty();
}

/**
* @brief
*     Determine whether the given index is a valid index of the underlying vector.
*/
template<class T>
inline bool scan::List<T>::valid_index(const ptrdiff_t &t_idx) const
{
    ptrdiff_t count{ static_cast<ptrdiff_t>(size()) };
    return t_idx >= 0 ? t_idx < count : std::abs(t_idx) <= count;
}

/**
* @brief
*     Find the index of the first matching element in the underlying vector.
*/
template<class T>
inline size_t scan::List<T>::find(const value_type &t_elem) const
{
    const iterator iter{ ranges::find(*this, t_elem) };
    return iter == end() ? NPOS : static_cast<size_t>(ranges::distance(begin(), iter));
}

/**
* @brief
*     Get the current size of the underlying vector.
*/
template<class T>
inline size_t scan::List<T>::size() const noexcept
{
    return m_buffer.size();
}

/**
* @brief
*     Get a constant pointer to the array of the underlying vector.
*/
template<class T>
inline const typename scan::List<T>::value_type *scan::List<T>::data() const noexcept
{
    return m_buffer.data();
}

/**
* @brief
*     Get a pointer to the array of the underlying vector.
*/
template<class T>
inline typename scan::List<T>::value_type *scan::List<T>::data() noexcept
{
    return m_buffer.data();
}

/**
* @brief
*     Get a constant iterator to the first element in the underlying vector.
*/
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::begin() const noexcept
{
    return data();
}

/**
* @brief
*     Get a constant iterator to the past-the-end element in the underlying vector. 
*/
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::end() const noexcept
{
    return begin() + size();
}

/**
* @brief
*     Join the underlying elements using the given delimiter.
*/
template<class T>
inline std::string scan::List<T>::join(const string &t_sep) const requires LShift<T>
{
    return algo::join(*this, t_sep);
}

/**
* @brief
*     Join the underlying elements using a line feed delimiter.
*/
template<class T>
inline std::string scan::List<T>::join_lines() const requires LShift<T>
{
    return join(LF);
}

/**
* @brief
*     Get a constant reference to the element located at the given vector index.
*/
template<class T>
inline const T &scan::List<T>::at(const ptrdiff_t &t_idx) const
{
    (*this)[t_idx];
}

/**
* @brief
*     Get a reference to the element located at the given vector index.
*/
template<class T>
inline T &scan::List<T>::at(const ptrdiff_t &t_idx)
{
    (*this)[t_idx];
}

/**
* @brief
*     Get a constant reference to the last element in the underlying vector.
*/
template<class T>
inline const T &scan::List<T>::last() const
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
template<class T>
inline T &scan::List<T>::last()
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
template<class T>
inline const std::vector<T> &scan::List<T>::vector() const noexcept
{
    return m_buffer;
}

/**
* @brief
*     Get a reference to the underlying vector.
*/
template<class T>
inline std::vector<T> &scan::List<T>::vector() noexcept
{
    return m_buffer;
}

/**
* @brief
*     Make a copy of the current list object.
*/
template<class T>
inline scan::List<T> scan::List<T>::copy() const noexcept
{
    return *this;
}

/**
* @brief
*     Retrieve a subrange of the underlying elements
*     based on the given start and end list iterators.
*/
template<class T>
inline scan::List<T> scan::List<T>::slice(const iterator &t_beg,
                                          const iterator &t_end) const
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
template<class T>
inline scan::List<T> scan::List<T>::slice(const size_t &t_beg_idx,
                                          const size_t &t_end_idx) const
{
    return slice(begin() + t_beg_idx, t_end_idx == NPOS ? end() : begin() + t_end_idx);
}

/**
* @brief
*     Determine whether the given iterator is a valid iterator of the underlying vector.
*/
template<class T>
inline bool scan::List<T>::valid_iterator(const iterator &t_iter) const
{
    return t_iter >= begin() && t_iter <= end();
}

#endif // !SCAN_LIST_H
