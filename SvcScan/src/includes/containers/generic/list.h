/*
*  list.h
*  ------
*  Header file for a generic container with an underlying vector
*/
#pragma once

#ifndef LIST_H
#define LIST_H

#include "../../concepts/type_concepts.h"
#include "../../errors/arg_ex.h"
#include "../../io/std_util.h"
#include "../../utils/algorithm.h"
#include "../../utils/type_defs.h"
#include "iterator.h"

namespace scan
{
    /**
    * @brief  Generic container that encapsulates a vector.
    */
    template<class T>
    class List
    {
    public:  /* Type Aliases */
        using value_type     = T;
        using const_iterator = Iterator<value_type>;
        using iterator       = const_iterator;

    private:  /* Type Aliases */
        using algo     = Algorithm;
        using vector_t = vector<value_type>;

    private:  /* Constants */
        static constexpr size_t NPOS = -1;  // Max collection size

    private:  /* Fields */
        vector_t m_vect;  // Underlying vector

    public:  /* Constructors & Destructor */
        List() = default;
        List(const List &t_list) noexcept;
        List(List &&) = default;

        template<Range R>
        List(const R &t_range);

        template<class ...Args>
        List(const Args &...t_args);

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

        template<class ...Args>
        void add(const Args &...t_args);

        template<class ...Args>
        void add(Args &&...t_args);

        template<Range R>
        void add_range(const R &t_range);

        void clear();
        void remove(const value_type &t_elem);
        void remove_at(const size_t &t_offset);
        void shrink_to_fit();

        template<class ...Args>
        bool any(const Args &...t_args) const;

        bool contains(const value_type &t_elem) const;
        bool empty() const noexcept;

        size_t find(const value_type &t_elem,
                    const size_t &t_start_pos = 0,
                    const size_t &t_add_offset = 0) const;

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
        List slice(const iterator &t_begin, const iterator &t_end) const;

    private:  /* Methods */
        bool valid_index(const ptrdiff_t &t_idx) const;
    };
}

/**
* @brief  Initialize the object.
*/
template<class T>
inline scan::List<T>::List(const List &t_list) noexcept
{
    *this = t_list;
}

/**
* @brief  Initialize the object.
*/
template<class T>
template<scan::Range R>
inline scan::List<T>::List(const R &t_range)
{
    add_range(t_range);
}

/**
* @brief  Initialize the object.
*/
template<class T>
template<class ...Args>
inline scan::List<T>::List(const Args &...t_args)
{
    static_assert(sizeof...(t_args) > 0);
    add(t_args...);
}

/**
* @brief  Copy assignment operator overload.
*/
template<class T>
inline scan::List<T> &scan::List<T>::operator=(const List &t_list) noexcept
{
    m_vect = t_list.m_vect;
    return *this;
}

/**
* @brief  Cast operator overload.
*/
template<class T>
inline scan::List<T>::operator vector_t() const noexcept
{
    return m_vect;
}

/**
* @brief  Subscript operator overload.
*/
template<class T>
inline T &scan::List<T>::operator[](const ptrdiff_t &t_idx)
{
    if (!valid_index(t_idx))
    {
        throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
    }
    return m_vect.at(t_idx >= 0 ? t_idx : size() - std::abs(t_idx));
}

/**
* @brief  Subscript operator overload.
*/
template<class T>
inline const T &scan::List<T>::operator[](const ptrdiff_t &t_idx) const
{
    if (!valid_index(t_idx))
    {
        throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
    }
    return m_vect.at(t_idx >= 0 ? t_idx : size() - std::abs(t_idx));
}

/**
* @brief  Create a list that contains all integers within the given range bounds.
*/
template<class T>
inline scan::List<T> scan::List<T>::fill(const T &t_min,
                                         const T &t_max) requires std::integral<T> {
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
* @brief  Add the given element to the underlying vector.
*/
template<class T>
inline void scan::List<T>::add(const value_type &t_elem)
{
    m_vect.push_back(t_elem);
}

/**
* @brief  Add the given elements to the underlying vector.
*/
template<class T>
template<class ...Args>
inline void scan::List<T>::add(const Args &...t_args)
{
    static_assert(sizeof...(t_args) > 0);
    (m_vect.push_back(t_args), ...);
}

/**
* @brief  Add the given elements to the underlying vector.
*/
template<class T>
template<class ...Args>
inline void scan::List<T>::add(Args &&...t_args)
{
    static_assert(sizeof...(t_args) > 0);
    m_vect.emplace_back(std::forward<Args>(t_args)...);
}

/**
* @brief  Add the given range of elements to the underlying vector.
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
* @brief  Remove all elements from the underlying vector.
*/
template<class T>
inline void scan::List<T>::clear()
{
    algo::clear(m_vect);
}

/**
* @brief  Remove the first matching element in the underlying vector.
*/
template<class T>
inline void scan::List<T>::remove(const value_type &t_elem)
{
    const size_t offset{ find(t_elem) };

    if (offset == NPOS)
    {
        throw ArgEx{ "t_elem", "No matching element found to remove" };
    }

    m_vect.erase(m_vect.begin() + offset);
    shrink_to_fit();
}

/**
* @brief  Remove the underlying vector element at the given index.
*/
template<class T>
inline void scan::List<T>::remove_at(const size_t &t_offset)
{
    if (t_offset >= size())
    {
        throw ArgEx{ "t_offset", "Index is out of the underlying vector bounds" };
    }

    m_vect.erase(m_vect.begin() + t_offset);
    shrink_to_fit();
}

/**
* @brief  Request that unused capacity memory in the underlying vector be released.
*/
template<class T>
inline void scan::List<T>::shrink_to_fit()
{
    m_vect.shrink_to_fit();
}

/**
* @brief  Determine whether the underlying vector contains any of the given elements.
*/
template<class T>
template<class ...Args>
inline bool scan::List<T>::any(const Args &...t_args) const
{
    return (contains(t_args) || ...);
}

/**
* @brief  Determine whether the underlying vector contains the given element.
*/
template<class T>
inline bool scan::List<T>::contains(const value_type &t_elem) const
{
    return find(t_elem) != NPOS;
}

/**
* @brief  Determine whether the underlying vector is empty.
*/
template<class T>
inline bool scan::List<T>::empty() const noexcept
{
    return m_vect.empty();
}

/**
* @brief  Find the index of the first matching element in the underlying vector.
*/
template<class T>
inline size_t scan::List<T>::find(const value_type &t_elem,
                                  const size_t &t_start_pos,
                                  const size_t &t_add_offset) const {

    const iterator iter{ ranges::find(begin() + t_start_pos, end(), t_elem) };
    return iter == end() ? NPOS : algo::distance(*this, iter) + t_add_offset;
}

/**
* @brief  Get the current size of the underlying vector.
*/
template<class T>
inline size_t scan::List<T>::size() const noexcept
{
    return m_vect.size();
}

/**
* @brief  Get a constant pointer to the array of the underlying vector.
*/
template<class T>
inline const typename scan::List<T>::value_type *scan::List<T>::data() const noexcept
{
    return m_vect.data();
}

/**
* @brief  Get a pointer to the array of the underlying vector.
*/
template<class T>
inline typename scan::List<T>::value_type *scan::List<T>::data() noexcept
{
    return m_vect.data();
}

/**
* @brief  Get a constant iterator to the first element in the underlying vector.
*/
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::begin() const noexcept
{
    return data();
}

/**
* @brief  Get a constant iterator to the past-the-end element in
*         the underlying vector. 
*/
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::end() const noexcept
{
    return begin() + size();
}

/**
* @brief  Join the underlying elements using the given delimiter.
*/
template<class T>
inline std::string scan::List<T>::join(const string &t_sep) const requires LShift<T>
{
    return algo::join(*this, t_sep);
}

/**
* @brief  Join the underlying elements using a line feed delimiter.
*/
template<class T>
inline std::string scan::List<T>::join_lines() const requires LShift<T>
{
    return join(&LF[0]);
}

/**
* @brief  Get a constant reference to the element located at the given vector index.
*/
template<class T>
inline const T &scan::List<T>::at(const ptrdiff_t &t_idx) const
{
    (*this)[t_idx];
}

/**
* @brief  Get a reference to the element located at the given vector index.
*/
template<class T>
inline T &scan::List<T>::at(const ptrdiff_t &t_idx)
{
    (*this)[t_idx];
}

/**
* @brief  Get a constant reference to the last element in the underlying vector.
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
* @brief  Get a reference to the last element in the underlying vector.
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
* @brief  Get a constant reference to the underlying vector.
*/
template<class T>
inline const std::vector<T> &scan::List<T>::vector() const noexcept
{
    return m_vect;
}

/**
* @brief  Get a reference to the underlying vector.
*/
template<class T>
inline std::vector<T> &scan::List<T>::vector() noexcept
{
    return m_vect;
}

/**
* @brief  Make a copy of the current list object.
*/
template<class T>
inline scan::List<T> scan::List<T>::copy() const noexcept
{
    return *this;
}

/**
* @brief  Retrieve a range of elements from the underlying vector.
*/
template<class T>
inline scan::List<T> scan::List<T>::slice(const iterator &t_begin,
                                          const iterator &t_end) const {
    List list;

    for (iterator it{ t_begin }; it != t_end; ++it)
    {
        list.add(*it);
    }
    return list;
}

/**
* @brief  Determine if the given index is valid for the underlying vector.
*/
template<class T>
inline bool scan::List<T>::valid_index(const ptrdiff_t &t_idx) const
{
    ptrdiff_t count{ static_cast<ptrdiff_t>(size()) };
    return t_idx >= 0 ? t_idx < count : std::abs(t_idx) <= count;
}

#endif // !LIST_H
