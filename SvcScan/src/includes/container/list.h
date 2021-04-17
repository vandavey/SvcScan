/*
*  list.h
*  ------
*  Header file for generic container with underlying vector
*/
#pragma once

#ifndef LIST_H
#define LIST_H

#include <string>
#include <vector>
#include "../except/argex.h"
#include "../utils/util.h"
#include "iterator.h"

namespace scan
{
    /// ***
    /// Generic container that encapsulates a vector
    /// ***
    template<class T>
    class List
    {
    public:  /* Types */
        using value_type = T;
        using iterator = Iterator<value_type>;

    private:  /* Types */
        using string = std::string;
        using vector_st = std::vector<size_t>;
        using vector_t = std::vector<value_type>;

    private:  /* Constants & Fields */
        static constexpr char LF[]{ *Util::LF, '\0' };

        vector_t m_vect;  // Underlying vector

    public:  /* Constructors & Destructor */
        List() = default;
        List(const List &t_list);
        explicit List(const vector_t &t_vect);

        virtual ~List() = default;

    public:  /* Operators */
        operator const vector_t() const noexcept;

        T &operator[](const size_t &t_idx);
        List &operator=(const vector_t &t_vect) noexcept;

    public:  /* Methods */
        static const bool contains(const vector_t &t_vect,
                                   const value_type &t_elem);

        static const string join(const vector_t &t_vect,
                                 const string &t_delim = LF);

        void add(const value_type &t_elem);
        void remove(const value_type &t_elem);
        void remove(const size_t &t_offset);

        const bool any(const vector_t &t_vect) const noexcept;
        const bool contains(const value_type &t_elem) const noexcept;
        const bool empty() const noexcept;

        const size_t index_of(const value_type &t_elem) const noexcept;
        const size_t size() const noexcept;

        const string join(const string &t_delim = LF) const;

        typename iterator begin() const noexcept;
        typename iterator end() const noexcept;

        const T &at(const size_t &t_idx) const;
        const T last() const;

        const List<T> slice(const iterator &t_beg, const iterator &t_end) const;

    private:  /* Methods */
        const bool valid_index(const size_t &t_idx) const;
        const bool valid_index(const vector_st &t_vect) const;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::List<T>::List(const List &t_list)
{
    m_vect = t_list.m_vect;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::List<T>::List(const vector_t &t_vect)
{
    m_vect = t_vect;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::List<T>::operator const vector_t() const noexcept
{
    return static_cast<vector_t>(m_vect);
}

/// ***
/// Subscript operator overload
/// ***
template<class T>
inline T &scan::List<T>::operator[](const size_t &t_idx)
{
    if (!valid_index(t_idx))
    {
        throw ArgEx("t_idx", "Index out of vector bounds");
    }
    return static_cast<value_type &>(m_vect[t_idx]);
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline scan::List<T> &scan::List<T>::operator=(const vector_t &t_vect) noexcept
{
    m_vect = t_vect;
    return *this;
}

/// ***
/// Utility method - Determine if vector contains the element
/// ***
template<class T>
inline const bool scan::List<T>::contains(const vector_t &t_vect,
                                          const value_type &t_elem) {
    return List(t_vect).contains(t_elem);
}

/// ***
/// Utility method - Join vector elements by given delimiter
/// ***
template<class T>
inline const std::string scan::List<T>::join(const vector_t &t_vect,
                                             const string &t_delim) {
    return List(t_vect).join(t_delim);
}

/// ***
/// Add an element to the underlying vector
/// ***
template<class T>
inline void scan::List<T>::add(const value_type &t_elem)
{
    m_vect.push_back(t_elem);
}

/// ***
/// Remove the first matching element in the underlying vector
/// ***
template<class T>
inline void scan::List<T>::remove(const value_type &t_elem)
{
    const ptrdiff_t offset{ static_cast<ptrdiff_t>(index_of(t_elem)) };

    // No matching element found
    if (offset < 0)
    {
        throw ArgEx("t_elem", "No matching element found to remove");
    }
    remove(static_cast<size_t>(offset));
}

/// ***
/// Remove vector element specified by the iterator
/// ***
template<class T>
inline void scan::List<T>::remove(const size_t &t_offset)
{
    // Index out of vector bounds
    if (t_offset >= size())
    {
        throw ArgEx("t_offset", "Index out of vector bounds");
    }

    m_vect.erase(m_vect.begin() + t_offset);
    m_vect.shrink_to_fit();
}

/// ***
/// Determine if underlying vector contains any of the given elements
/// ***
template<class T>
inline const bool scan::List<T>::any(const vector_t &t_vect) const noexcept
{
    if (t_vect.empty())
    {
        return false;
    }

    // Look for matching element
    for (const value_type &elem : t_vect)
    {
        if (contains(elem))
        {
            return true;
        }
    }
    return false;
}

/// ***
/// Determine if underlying vector contains the given element
/// ***
template<class T>
inline const bool scan::List<T>::contains(const value_type &t_elem) const
noexcept {
    return index_of(t_elem) != -1;
}

/// ***
/// Determine if underlying vector is empty
/// ***
template<class T>
inline const bool scan::List<T>::empty() const noexcept
{
    return m_vect.empty();
}

/// ***
/// Get the index of first matching element in the vector
/// ***
template<class T>
inline const size_t scan::List<T>::index_of(const value_type &t_elem) const
noexcept {
    for (size_t i{ 0 }; i < size(); i++)
    {
        if (m_vect[i] == t_elem)
        {
            return i;
        }
    }
    return static_cast<size_t>(-1);
}

/// ***
/// Get the current size of the underlying vector
/// ***
template<class T>
inline const size_t scan::List<T>::size() const noexcept
{
    return m_vect.size();
}

/// ***
/// Join current list elements by given separator (default: EOL)
/// ***
template<class T>
inline const std::string scan::List<T>::join(const string &t_delim) const
{
    string data;

    // Append vector arguments to string
    for (const value_type &elem : m_vect)
    {
        data += static_cast<string>(elem);

        // Don't append separator to last arg
        if (elem != last())
        {
            data += t_delim;
        }
    }
    return data;
}

/// ***
/// Get iterator to first element in underlying vector
/// ***
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::begin() const noexcept
{
    return static_cast<iterator>(m_vect.data());
}

/// ***
/// Get iterator to past-the-end element in underlying vector
/// ***
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::end() const noexcept
{
    return static_cast<iterator>(m_vect.data() + size());
}

/// ***
/// Get element reference at the given index
/// ***
template<class T>
inline const T &scan::List<T>::at(const size_t &t_idx) const
{
    if (!valid_index(t_idx))
    {
        throw ArgEx("t_idx", "Index out of vector bounds");
    }
    return m_vect.at(t_idx);
}

/// ***
/// Get the last element in the underlying vector
/// ***
template<class T>
inline const T scan::List<T>::last() const
{
    return empty() ? value_type() : m_vect.at(size() - 1);
}

/// ***
/// Retrieve a range of elements from the underlying vector
/// ***
template<class T>
inline const scan::List<T> scan::List<T>::slice(const iterator &t_beg,
                                                const iterator &t_end) const {
    List range;

    // Add elements to the range
    for (iterator it{ t_beg }; it != t_end; ++it)
    {
        range.add(*it);
    }
    return range;
}

/// ***
/// Determine if index is valid for the underlying vector
/// ***
template<class T>
inline const bool scan::List<T>::valid_index(const size_t &t_idx) const
{
    return (t_idx >= 0) && (t_idx < size());
}

/// ***
/// Determine if index is valid for the underlying vector
/// ***
template<class T>
inline const bool scan::List<T>::valid_index(const vector_st &t_vect) const
{
    // Validate each index in vector
    for (const size_t &idx : t_vect)
    {
        if (!valid_index(idx))
        {
            return false;
        }
    }
    return true;
}

#endif // !LIST_H
