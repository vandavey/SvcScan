/*
*  list.h
*  ------
*  Header file for generic container with underlying vector
*/
#pragma once

#ifndef LIST_H
#define LIST_H

#include <array>
#include <string>
#include <vector>
#include "../conio/stdutil.h"
#include "../except/argex.h"
#include "../utils/util.h"
#include "citerator.h"
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

        using iterator       = Iterator<value_type>;
        using const_iterator = CIterator<value_type>;

    private:  /* Types */
        using string    = std::string;
        using vector_t  = std::vector<value_type>;
        using init_list = std::initializer_list<value_type>;

        template<size_t N>
        using array_t = std::array<value_type, N>;

    private:  /* Constants */
        static constexpr char LF[]{ *StdUtil::LF, '\0' };  // EOL (line feed)

        static constexpr size_t NPOS{ string::npos };      // Element not found

    private:  /* Fields */
        vector_t m_vect;  // Underlying vector

    public:  /* Constructors & Destructor */
        List() = default;
        List(const List &t_list);
        explicit List(const init_list &t_il);
        explicit List(const vector_t &t_vect);

        virtual ~List() = default;

    public:  /* Operators */
        operator vector_t() const noexcept;

        T &operator[](const size_t &t_idx);
        List &operator=(const vector_t &t_vect) noexcept;

    public:  /* Methods */
        static bool contains(const vector_t &t_vect, const value_type &t_elem);

        static string join(const vector_t &t_vect, const string &t_delim = LF);

        void add(const value_type &t_elem);
        void add_range(const vector_t &t_vect);
        void clear();
        void remove(const value_type &t_elem);
        void remove(const size_t &t_offset);

        bool any(const vector_t &t_vect) const noexcept;
        bool contains(const value_type &t_elem) const noexcept;
        bool empty() const noexcept;

        size_t index_of(const value_type &t_elem,
                        const size_t &t_offset = 0) const noexcept;

        size_t size() const noexcept;

        string join(const string &t_delim = LF) const;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        iterator begin() noexcept;
        iterator end() noexcept;

        const T &at(const size_t &t_idx) const;
        T last() const;

        List slice(const iterator &t_beg, const iterator &t_end) const;

    private:  /* Methods */
        bool valid_index(const size_t &t_idx) const;
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
inline scan::List<T>::List(const init_list &t_il)
{
    m_vect = t_il;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::List<T>::List(const vector_t &t_vect) : m_vect(t_vect)
{
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::List<T>::operator vector_t() const noexcept
{
    return m_vect;
}

/// ***
/// Subscript operator overload
/// ***
template<class T>
inline T &scan::List<T>::operator[](const size_t &t_idx)
{
    if (!valid_index(t_idx))
    {
        throw ArgEx("t_idx", "Index is out of the vector bounds");
    }
    return m_vect.at(t_idx);
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
/// Utility - Determine if vector contains the element
/// ***
template<class T>
inline bool scan::List<T>::contains(const vector_t &t_vect, const value_type &t_elem)
{
    return List(t_vect).contains(t_elem);
}

/// ***
/// Utility - Join vector elements by given delimiter
/// ***
template<class T>
inline std::string scan::List<T>::join(const vector_t &t_vect, const string &t_delim)
{
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
/// Add a range of elements to the underlying vector
/// ***
template<class T>
inline void scan::List<T>::add_range(const vector_t &t_vect)
{
    for (const value_type &elem : t_vect)
    {
        m_vect.push_back(elem);
    }
}

/// ***
/// Remove all elements from the underlying vector
/// ***
template<class T>
inline void scan::List<T>::clear()
{
    m_vect.clear();
    m_vect.shrink_to_fit();
}

/// ***
/// Remove the first matching element in the underlying vector
/// ***
template<class T>
inline void scan::List<T>::remove(const value_type &t_elem)
{
    const size_t offset{ index_of(t_elem) };

    // No matching element found
    if (offset == NPOS)
    {
        throw ArgEx("t_elem", "No matching element found to remove");
    }

    m_vect.erase(m_vect.begin() + offset);
    m_vect.shrink_to_fit();
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
        throw ArgEx("t_offset", "Index is out of the vector bounds");
    }

    m_vect.erase(m_vect.begin() + t_offset);
    m_vect.shrink_to_fit();
}

/// ***
/// Determine if underlying vector contains any of the given elements
/// ***
template<class T>
inline bool scan::List<T>::any(const vector_t &t_vect) const noexcept
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
inline bool scan::List<T>::contains(const value_type &t_elem) const noexcept
{
    return index_of(t_elem) != NPOS;
}

/// ***
/// Determine if underlying vector is empty
/// ***
template<class T>
inline bool scan::List<T>::empty() const noexcept
{
    return m_vect.empty();
}

/// ***
/// Get the index of first matching element in the vector
/// ***
template<class T>
inline size_t scan::List<T>::index_of(const value_type &t_elem,
                                      const size_t &t_offset) const noexcept {
    size_t match_pos{ NPOS };

    for (size_t i{ 0 }; i < size(); i++)
    {
        if (m_vect[i] == t_elem)
        {
            match_pos = i;
        }
    }

    // Offset match index if specified
    return match_pos + t_offset;
}

/// ***
/// Get the current size of the underlying vector
/// ***
template<class T>
inline size_t scan::List<T>::size() const noexcept
{
    return m_vect.size();
}

/// ***
/// Join current list elements by given separator (default: EOL)
/// ***
template<class T>
inline std::string scan::List<T>::join(const string &t_delim) const
{
    static_assert(std::is_convertible_v<T, std::string>);

    string data;

    // Append vector arguments to string
    for (size_t i{ 0 }; i < m_vect.size(); i++)
    {
        data += static_cast<string>(m_vect[i]);

        // Append separator between elements
        if (i != m_vect.size() - 1)
        {
            data += t_delim;
        }
    }
    return data;
}

/// ***
/// Get constant iterator to first element in underlying vector
/// ***
template<class T>
inline typename scan::List<T>::const_iterator scan::List<T>::cbegin() const noexcept
{
    return static_cast<const_iterator>(m_vect.data());
}

/// ***
/// Get constant iterator to past-the-end element in underlying vector
/// ***
template<class T>
inline typename scan::List<T>::const_iterator scan::List<T>::cend() const noexcept
{
    return static_cast<const_iterator>(m_vect.data() + size());
}

/// ***
/// Get iterator to first element in underlying vector
/// ***
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::begin() noexcept
{
    return static_cast<iterator>(m_vect.data());
}

/// ***
/// Get iterator to past-the-end element in underlying vector
/// ***
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::end() noexcept
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
        throw ArgEx("t_idx", "Index is out of the vector bounds");
    }
    return m_vect.at(t_idx);
}

/// ***
/// Get the last element in the underlying vector
/// ***
template<class T>
inline T scan::List<T>::last() const
{
    return empty() ? value_type() : m_vect.at(size() - 1);
}

/// ***
/// Retrieve a range of elements from the underlying vector
/// ***
template<class T>
inline scan::List<T> scan::List<T>::slice(const iterator &t_beg,
                                          const iterator &t_end) const {
    List lrange;

    // Add elements to the range
    for (iterator it{ t_beg }; it != t_end; ++it)
    {
        lrange.add(*it);
    }
    return lrange;
}

/// ***
/// Determine if index is valid for the underlying vector
/// ***
template<class T>
inline bool scan::List<T>::valid_index(const size_t &t_idx) const
{
    return (t_idx >= 0) && (t_idx < size());
}

#endif // !LIST_H
