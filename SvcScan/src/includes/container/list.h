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
#include "fwditerator.h"

namespace Scan
{
    /// ***
    /// Generic container that encapsulates a vector
    /// ***
    template<class T>
    class List
    {
    public:  /* Types */
        using value_type = T;
        using iterator = FwdIterator<value_type>;

    private:  /* Types */
        using string = std::string;
        using vector_t = std::vector<value_type>;

    private:  /* Constants & Fields */
        static constexpr char LF[] = {*Util::LF};

        vector_t m_vect;  // Underlying vector

    public:  /* Constructors & Destructor */
        List() = default;
        List(const List &list);
        explicit List(const vector_t &vect);

        virtual ~List() = default;

    public:  /* Operators */
        operator const vector_t() const noexcept;

        const T operator[](const size_t &index) const;
        T &operator[](const size_t &index);

        List &operator=(const vector_t &vect) noexcept;
        List &operator+=(const value_type &elem);

    public:  /* Methods */
        static const bool contains(const vector_t &vect,
                                   const value_type &elem);

        static const string join(const vector_t &vect,
                                 const string &delim = LF);

        void remove(const value_type &elem);
        void remove(const size_t &offset);

        const bool any(const vector_t &vect) const noexcept;
        const bool contains(const value_type &elem) const noexcept;
        const bool empty() const noexcept;

        const ptrdiff_t index_of(const value_type &elem) const noexcept;
        const size_t size() const noexcept;

        const string join(const string &delim = LF) const;

        iterator begin() const noexcept;
        iterator end() const noexcept;

        const value_type last() const;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::List<T>::List(const List &list)
{
    this->m_vect = list.m_vect;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline Scan::List<T>::List(const vector_t &vect)
{
    this->m_vect = vect;
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline Scan::List<T>::operator const vector_t() const noexcept
{
    return static_cast<vector_t>(m_vect);
}

/// ***
/// Subscript operator overload
/// ***
template<class T>
inline const T Scan::List<T>::operator[](const size_t &index) const
{
    if (index >= size())
    {
        throw ArgEx("index", "Index out of vector bounds");
    }
    return static_cast<value_type>(m_vect[index]);
}

/// ***
/// Subscript operator overload
/// ***
template<class T>
inline T &Scan::List<T>::operator[](const size_t &index)
{
    if (index >= size())
    {
        throw ArgEx("index", "Index out of vector bounds");
    }
    return static_cast<value_type &>(m_vect[index]);
}

/// ***
/// Assignment operator overload
/// ***
template<class T>
inline Scan::List<T> &Scan::List<T>::operator=(const vector_t &vect) noexcept
{
    m_vect = vect;
    return *this;
}

/// ***
/// Compound assignment operator overload
/// ***
template<class T>
inline Scan::List<T> &Scan::List<T>::operator+=(const value_type &elem)
{
    m_vect.push_back(static_cast<value_type>(elem));
    return *this;
}

/// ***
/// Utility method - Determine if vector contains the element
/// ***
template<class T>
inline const bool Scan::List<T>::contains(const vector_t &vect,
                                          const value_type &elem) {
    return List(vect).contains(elem);
}

/// ***
/// Utility method - Join vector elements by given delimiter
/// ***
template<class T>
inline const std::string Scan::List<T>::join(const vector_t &vect,
                                             const string &delim) {
    return List(vect).join(delim);
}

/// ***
/// Remove the first matching element in the vector
/// ***
template<class T>
inline void Scan::List<T>::remove(const value_type &elem)
{
    const ptrdiff_t offset = {index_of(elem)};

    // No matching element found
    if (offset < 0)
    {
        throw ArgEx("elem", "No matching element found to remove");
    }
    remove(static_cast<size_t>(offset));
}

/// ***
/// Remove vector element specified by the iterator
/// ***
template<class T>
inline void Scan::List<T>::remove(const size_t &offset)
{
    // Index out of vector bounds
    if (offset >= size())
    {
        throw ArgEx("offset", "Index out of vector bounds");
    }

    m_vect.erase(m_vect.begin() + offset);
    m_vect.shrink_to_fit();
}

/// ***
/// Determine if underlying vector contains any of the given elements
/// ***
template<class T>
inline const bool Scan::List<T>::any(const vector_t &vect) const noexcept
{
    if (vect.empty())
    {
        return false;
    }

    // Look for matching element
    for (const value_type &elem : vect)
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
inline const bool Scan::List<T>::contains(const value_type &elem) const noexcept
{
    return index_of(elem) != -1;
}

/// ***
/// Determine if underlying vector is empty
/// ***
template<class T>
inline const bool Scan::List<T>::empty() const noexcept
{
    return m_vect.empty();
}

/// ***
/// Get the index of first matching element in the vector
/// ***
template<class T>
inline const ptrdiff_t Scan::List<T>::index_of(const value_type &elem) const noexcept
{
    for (size_t i = {0}; i < size(); i++)
    {
        if (m_vect[i] == elem)
        {
            return static_cast<ptrdiff_t>(i);
        }
    }
    return static_cast<ptrdiff_t>(-1);
}

/// ***
/// Get the current size of the underlying vector
/// ***
template<class T>
inline const size_t Scan::List<T>::size() const noexcept
{
    return m_vect.size();
}

/// ***
/// Join current list elements by given separator (default: EOL)
/// ***
template<class T>
inline const std::string Scan::List<T>::join(const string &delim) const
{
    string data;
    const size_t len = {size()};

    // Append vector arguments to string
    for (const value_type &elem : m_vect)
    {
        data += static_cast<string>(elem);

        // Don't append separator to last arg
        if (elem != last())
        {
            data += delim;
        }
    }
    return static_cast<string>(data);
}

/// ***
/// Get iterator to first element in underlying vector
/// ***
template<class T>
inline typename Scan::List<T>::iterator Scan::List<T>::begin() const noexcept
{
    return static_cast<iterator>(m_vect.data());
}

/// ***
/// Get iterator to past-the-end element in underlying vector
/// ***
template<class T>
inline typename Scan::List<T>::iterator Scan::List<T>::end() const noexcept
{
    return static_cast<iterator>(m_vect.data() + size());
}

/// ***
/// Get the last element in the underlying vector
/// ***
template<class T>
inline const typename Scan::List<T>::value_type Scan::List<T>::last() const
{
    return empty() ? value_type() : m_vect[size() - 1];
}

#endif // !LIST_H
