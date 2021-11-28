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
#include "../../conio/stdutil.h"
#include "../../except/argex.h"
#include "../../utils/util.h"
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
        List &operator=(const vector_t &t_vect) noexcept;

        operator vector_t() const noexcept;

        T &operator[](const ptrdiff_t &t_idx);
        const T &operator[](const ptrdiff_t &t_idx) const;

    public:  /* Methods */
        static bool any(const vector_t &t_vect, const vector_t &t_elements) noexcept;
        static bool contains(const vector_t &t_vect, const value_type &t_elem);

        static string join(const vector_t &t_vect, const string &t_delim = string());
        static string join_lines(const vector_t &t_vect);

        void add(const value_type &t_elem);
        void add_range(const vector_t &t_vect);
        void clear();
        void remove(const value_type &t_elem);
        void remove_at(const size_t &t_offset);
        void shrink_to_fit();

        bool any(const vector_t &t_vect) const noexcept;
        bool contains(const value_type &t_elem) const noexcept;
        bool empty() const noexcept;

        size_t find(const value_type &t_elem,
                    const size_t &t_start_pos = 0,
                    const size_t &t_add_offset = 0) const;

        size_t size() const noexcept;

        value_type *data() noexcept;
        const value_type *data() const noexcept;

        string join(const string &t_delim = LF) const;

        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        iterator begin() noexcept;
        iterator end() noexcept;

        const T &at(const ptrdiff_t &t_idx) const;
        T &at(const ptrdiff_t &t_idx);

        List slice(const const_iterator &t_cbeg, const const_iterator &t_cend) const;

    private:  /* Methods */
        bool valid_index(const ptrdiff_t &t_idx) const;
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
/// Assignment operator overload
/// ***
template<class T>
inline scan::List<T> &scan::List<T>::operator=(const vector_t &t_vect) noexcept
{
    m_vect = t_vect;
    return *this;
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
inline T &scan::List<T>::operator[](const ptrdiff_t &t_idx)
{
    return at(t_idx);
}

/// ***
/// Subscript operator overload
/// ***
template<class T>
inline const T &scan::List<T>::operator[](const ptrdiff_t &t_idx) const
{
    return at(t_idx);
}

/// ***
/// Utility - Determine if the given vector contains any of the specified elements
/// ***
template<class T>
inline bool scan::List<T>::any(const vector_t &t_vect,
                               const vector_t &t_elements) noexcept {

    return List(t_vect).any(t_elements);
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
/// Utility - Join the given vector elements using a line feed delimiter
/// ***
template<class T>
inline std::string scan::List<T>::join_lines(const vector_t &t_vect)
{
    return join(t_vect, LF);
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
    shrink_to_fit();
}

/// ***
/// Remove the first matching element in the underlying vector
/// ***
template<class T>
inline void scan::List<T>::remove(const value_type &t_elem)
{
    const size_t offset{ find(t_elem) };

    // No matching element found
    if (offset == NPOS)
    {
        throw ArgEx("t_elem", "No matching element found to remove");
    }

    m_vect.erase(m_vect.begin() + offset);
    shrink_to_fit();
}

/// ***
/// Remove the vector element at the specified index
/// ***
template<class T>
inline void scan::List<T>::remove_at(const size_t &t_offset)
{
    // Index out of vector bounds
    if (t_offset >= size())
    {
        throw ArgEx("t_offset", "Index is out of the underlying vector bounds");
    }

    m_vect.erase(m_vect.begin() + t_offset);
    shrink_to_fit();
}

/// ***
/// Request removal of unused capacity memory in the underlying vector
/// ***
template<class T>
inline void scan::List<T>::shrink_to_fit()
{
    m_vect.shrink_to_fit();
}

/// ***
/// Determine if the underlying vector contains any of the given elements
/// ***
template<class T>
inline bool scan::List<T>::any(const vector_t &t_vect) const noexcept
{
    bool match_found{ false };

    // Look for matching element
    if (t_vect.empty())
    {
        for (const value_type &elem : t_vect)
        {
            if (contains(elem))
            {
                match_found = true;
                break;
            }
        }
    }
    return match_found;
}

/// ***
/// Determine if the underlying vector contains the given element
/// ***
template<class T>
inline bool scan::List<T>::contains(const value_type &t_elem) const noexcept
{
    return find(t_elem) != NPOS;
}

/// ***
/// Determine if the underlying vector is empty
/// ***
template<class T>
inline bool scan::List<T>::empty() const noexcept
{
    return m_vect.empty();
}

/// ***
/// Find the index of the first matching element in the vector
/// ***
template<class T>
inline size_t scan::List<T>::find(const value_type &t_elem,
                                  const size_t &t_start_pos,
                                  const size_t &t_add_offset) const {
    size_t match_pos{ NPOS };

    // Find matching element
    for (size_t i{ t_start_pos }; i < size(); i++)
    {
        if (m_vect[i] == t_elem)
        {
            match_pos = i;
            break;
        }
    }

    // Add additional offset if match found
    if (match_pos != NPOS)
    {
        match_pos += t_add_offset;
    }
    return match_pos;
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
/// Get a direct pointer to the memory array of the underlying vector
/// ***
template<class T>
inline typename scan::List<T>::value_type *scan::List<T>::data() noexcept
{
    return m_vect.data();
}

/// ***
/// Get a direct constant pointer to the memory array of the underlying vector
/// ***
template<class T>
inline const typename scan::List<T>::value_type *scan::List<T>::data() const noexcept
{
    return m_vect.data();
}

/// ***
/// Join the underlying vector elements by the given separator (default: LF)
/// ***
template<class T>
inline std::string scan::List<T>::join(const string &t_delim) const
{
    static_assert(std::is_convertible_v<value_type, string>);

    std::stringstream ss;

    // Append vector arguments to string
    for (size_t i{ 0 }; i < m_vect.size(); i++)
    {
        ss << static_cast<string>(m_vect[i]).c_str();

        // Append separator between elements
        if (i != m_vect.size() - 1)
        {
            ss << t_delim;
        }
    }
    return ss.str();
}

/// ***
/// Get a constant iterator to the first element in the underlying vector
/// ***
template<class T>
inline typename scan::List<T>::const_iterator scan::List<T>::cbegin() const noexcept
{
    return static_cast<const_iterator>(data());
}

/// ***
/// Get a constant iterator to the past-the-end element in the underlying vector
/// ***
template<class T>
inline typename scan::List<T>::const_iterator scan::List<T>::cend() const noexcept
{
    return static_cast<const_iterator>(data() + size());
}

/// ***
/// Get an iterator to the first element in the underlying vector
/// ***
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::begin() noexcept
{
    return static_cast<iterator>(data());
}

/// ***
/// Get an iterator to the past-the-end element in the underlying vector
/// ***
template<class T>
inline typename scan::List<T>::iterator scan::List<T>::end() noexcept
{
    return static_cast<iterator>(data() + size());
}

/// ***
/// Get a constant reference to the element located at the given vector index
/// ***
template<class T>
inline const T &scan::List<T>::at(const ptrdiff_t &t_idx) const
{
    if (!valid_index(t_idx))
    {
        throw ArgEx("t_idx", "Index is out of the underlying vector bounds");
    }
    return m_vect.at((t_idx >= 0) ? t_idx : (size() - std::abs(t_idx)));
}

/// ***
/// Get a reference to the element located at the given vector index
/// ***
template<class T>
inline T &scan::List<T>::at(const ptrdiff_t &t_idx)
{
    if (!valid_index(t_idx))
    {
        throw ArgEx("t_idx", "Index is out of the underlying vector bounds");
    }
    return m_vect.at((t_idx >= 0) ? t_idx : (size() - std::abs(t_idx)));
}

/// ***
/// Retrieve a range of elements from the underlying vector
/// ***
template<class T>
inline scan::List<T> scan::List<T>::slice(const const_iterator &t_cbeg,
                                          const const_iterator &t_cend) const {
    List lbuffer;

    for (const_iterator it{ t_cbeg }; it != t_cend; ++it)
    {
        lbuffer.add(*it);
    }
    return lbuffer;
}

/// ***
/// Determine if the index is valid for the underlying vector
/// ***
template<class T>
inline bool scan::List<T>::valid_index(const ptrdiff_t &t_idx) const
{
    ptrdiff_t count(static_cast<size_t>(size()));

    // Validate positive/negative indices
    return (t_idx >= 0) ? (t_idx < count) : (std::abs(t_idx) <= count);
}

#endif // !LIST_H
