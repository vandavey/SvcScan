/*
*  list.h
*  ------
*  Header file for generic container with underlying vector
*/
#pragma once

#ifndef LIST_H
#define LIST_H

#include "../../constraints/typeconcepts.h"
#include "../../except/argex.h"
#include "../../io/stdutil.h"
#include "iterator.h"

namespace
{
    namespace ranges = std::ranges;
}

namespace scan
{
    /// ***
    /// Generic container that encapsulates a vector
    /// ***
    template<class T>
    class List
    {
    public:  /* Type Aliases */
        using value_type     = T;
        using const_iterator = Iterator<value_type>;

    private:  /* Type Aliases */
        using init_list = std::initializer_list<value_type>;
        using string    = std::string;
        using vector_t  = std::vector<value_type>;

    private:  /* Constants */
        static constexpr char LF[]{ *StdUtil::LF, '\0' };  // EOL (line feed)

        static constexpr size_t NPOS{ string::npos };      // Element not found

    private:  /* Fields */
        vector_t m_vect;  // Underlying vector

    public:  /* Constructors & Destructor */
        List() = default;
        List(const List &t_list);
        explicit List(const init_list &t_il);

        template<RangeT R>
        List(const R &t_range);

        virtual ~List() = default;

    public:  /* Operators */
        List &operator=(const vector_t &t_vect) noexcept;

        operator vector_t() const noexcept;

        T &operator[](const ptrdiff_t &t_idx);
        const T &operator[](const ptrdiff_t &t_idx) const;

    public:  /* Methods */
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

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        string join(const string &t_delim) const requires LShift<T>;
        string join_lines() const requires LShift<T>;

        const T &at(const ptrdiff_t &t_idx) const;
        T &at(const ptrdiff_t &t_idx);

        List copy() const noexcept;
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
template<scan::RangeT R>
inline scan::List<T>::List(const R &t_range)
{
    add_range(t_range);
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
        throw ArgEx{ "t_elem", "No matching element found to remove" };
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
        throw ArgEx{ "t_offset", "Index is out of the underlying vector bounds" };
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
    if (!t_vect.empty())
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

    using const_iterator_t = typename vector_t::const_iterator;

    const const_iterator_t iter = ranges::find(m_vect.cbegin() + t_start_pos,
                                               m_vect.cend(),
                                               t_elem);
    size_t offset{ NPOS };

    if (iter != m_vect.end())
    {
        const ptrdiff_t delta{ ranges::distance(m_vect.cbegin(), iter) };
        offset = static_cast<size_t>(delta) + t_add_offset;
    }
    return offset;
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
/// Get a constant iterator to the first element in the underlying vector
/// ***
template<class T>
inline typename scan::List<T>::const_iterator scan::List<T>::begin() const noexcept
{
    return static_cast<const_iterator>(data());
}

/// ***
/// Get a constant iterator to the past-the-end element in the underlying vector
/// ***
template<class T>
inline typename scan::List<T>::const_iterator scan::List<T>::end() const noexcept
{
    return static_cast<const_iterator>(data() + size());
}

/// ***
/// Join the underlying vector elements by the given separator (default: LF)
/// ***
template<class T>
inline std::string scan::List<T>::join(const string &t_sep) const requires LShift<T>
{
    std::stringstream ss;

    for (size_t i{ 0 }; i < size(); i++)
    {
        ss << at(i);

        // Append delimiter between elements
        if (i != m_vect.size() - 1)
        {
            ss << t_sep;
        }
    }
    return ss.str();
}

/// ***
/// Join the underlying elements using a line feed delimiter
/// ***
template<class T>
inline std::string scan::List<T>::join_lines() const requires LShift<T>
{
    return join(LF);
}

/// ***
/// Get a constant reference to the element located at the given vector index
/// ***
template<class T>
inline const T &scan::List<T>::at(const ptrdiff_t &t_idx) const
{
    if (!valid_index(t_idx))
    {
        throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
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
        throw ArgEx{ "t_idx", "Index is out of the underlying vector bounds" };
    }
    return m_vect.at((t_idx >= 0) ? t_idx : (size() - std::abs(t_idx)));
}

/// ***
/// Create a copy of the current list object
/// ***
template<class T>
inline scan::List<T> scan::List<T>::copy() const noexcept
{
    return *this;
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
