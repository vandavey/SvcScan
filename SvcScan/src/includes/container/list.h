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
        using string = std::string;

        using il_t       = std::initializer_list<value_type>;
        using vector_s_t = std::vector<size_t>;
        using vector_t   = std::vector<value_type>;

        template<size_t N>
        using array_t = std::array<value_type, N>;

    private:  /* Constants */
        static constexpr char LF[]{ *Util::LF, '\0' };  // EOL (line feed)

        static constexpr size_t NPOS{ string::npos };   // Element not found

    private:  /* Fields */
        vector_t m_vect;  // Underlying vector

    public:  /* Constructors & Destructor */
        List() = default;
        List(const List &t_list);
        explicit List(const il_t &t_il);
        explicit List(const vector_t &t_vect);

        virtual ~List() = default;

    public:  /* Operators */
        operator vector_t() const noexcept;

        T &operator[](const size_t &t_idx);
        List &operator=(const vector_t &t_vect) noexcept;

    public:  /* Methods */
        static bool contains(const vector_t &t_vect, const value_type &t_elem);

        static string join(const vector_t &t_vect, const string &t_delim = LF);

        template<size_t N>
        static array_t<N> copy_n(const il_t &t_il);

        void add(const value_type &t_elem);
        void add_range(const vector_t &t_vect);
        void clear();
        void remove(const value_type &t_elem);
        void remove(const size_t &t_offset);

        bool any(const vector_t &t_vect) const noexcept;
        bool contains(const value_type &t_elem) const noexcept;
        bool empty() const noexcept;

        size_t index_of(const value_type &t_elem) const noexcept;
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
        bool valid_index(const vector_s_t &t_vect) const;
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

// ***
/// Initialize the object
/// ***
template<class T>
inline scan::List<T>::List(const il_t &t_il)
{
    m_vect = { t_il };
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
        throw ArgEx("t_idx", "Index out of vector bounds");
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
    const List lbuffer{ t_vect };
    return lbuffer.contains(t_elem);
}

/// ***
/// Utility - Join vector elements by given delimiter
/// ***
template<class T>
inline std::string scan::List<T>::join(const vector_t &t_vect, const string &t_delim)
{
    const List lbuffer{ t_vect };
    return lbuffer.join(t_delim);
}

/// ***
/// Utility - Transform initializer_list into array container
/// ***
template<class T>
template<size_t N>
inline scan::List<T>::array_t<N> scan::List<T>::copy_n(const il_t &t_il)
{
    array_t<N> abuffer;

    if (N == 0)
    {
        return abuffer;
    }
    size_t i{ 0 };

    // Copy values into array buffer
    for (const value_type &elem : t_il)
    {
        abuffer[i++] = { elem };
    }
    return abuffer;
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
        throw ArgEx("t_offset", "Index out of vector bounds");
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
inline size_t scan::List<T>::index_of(const value_type &t_elem) const noexcept
{
    for (size_t i{ 0 }; i < size(); i++)
    {
        if (m_vect[i] == t_elem)
        {
            return i;
        }
    }
    return NPOS;
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
        throw ArgEx("t_idx", "Index out of vector bounds");
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
inline bool scan::List<T>::valid_index(const size_t &t_idx) const
{
    return (t_idx >= 0) && (t_idx < size());
}

/// ***
/// Determine if index is valid for the underlying vector
/// ***
template<class T>
inline bool scan::List<T>::valid_index(const vector_s_t &t_vect) const
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
