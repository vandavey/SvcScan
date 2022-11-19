/*
*  algorithm.h
*  -----------
*  Header file for range algorithms and utilities
*/
#pragma once

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <sdkddkver.h>
#include <boost/algorithm/string.hpp>
#include "../concepts/type_concepts.h"
#include "type_defs.h"

namespace scan
{
    /**
    * @brief  Range algorithms and utilities.
    */
    class Algorithm final
    {
    private:  /* Type Aliases */
        using str_iterator = string::const_iterator;
        using wstring      = std::wstring;

    public:  /* Constructors & Destructor */
        Algorithm() = delete;
        Algorithm(const Algorithm &) = delete;
        Algorithm(Algorithm &&) = delete;

        virtual ~Algorithm() = default;

    public:  /* Fields */
        static streamsize fstr_precision;  // Format string decimal precision

    public:  /* Operators */
        Algorithm &operator=(const Algorithm &) = default;
        Algorithm &operator=(Algorithm &&) = default;

    public:  /* Methods */
        template<ClearableRange R>
        static void clear_and_shrink(R &t_range);

        template<Range R>
        static bool empty(const R &t_range);

        static bool is_integral(const string &t_data);

        template<StringRange R>
        static bool is_integral(const R &t_range);

        static str_iterator find_nth(const string &t_data,
                                     const string &t_sub,
                                     const size_t &t_n,
                                     const bool &t_after = false);

        static size_t find_nth_pos(const string &t_data,
                                   const string &t_sub,
                                   const size_t &t_n,
                                   const bool &t_after = false);

        template<Range R, class T>
        static size_t count(const R &t_range,
                            const T &t_value) requires RangeValue<R, T>;

        static size_t count(const string &t_data, const string &t_sub) noexcept;

        template<Range R, RangeIterator T>
        static size_t distance(const R &t_range, const T &t_iter);

        template<RangeIterator T>
        static size_t distance(const T &t_beg_iter, const T &t_end_iter);

        template<LShift ...Args>
        static string concat(const Args &...t_args);

        static string erase(const string &t_data, const string &t_sub);

        template<LShift T, LShift ...Args>
        static string fstr(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        template<LShiftRange R>
        static string join(const R &t_range, const string &t_delim);

        static string replace(const string &t_data,
                              const string &t_old_sub,
                              const string &t_new_sub);

        template<StringRange R>
        static string replace(const string &t_data,
                              const R &t_old_subs,
                              const string &t_new_sub);

        static string str(const wstring &t_wdata);

        static string substr(const string &t_data,
                             const str_iterator &t_beg_it,
                             const str_iterator &t_end_it);

        static string to_lower(const string &t_data);

        template<LShift T>
        static string to_string(const T &t_obj);

        static string to_upper(const string &t_data);
        static string trim(const string &t_data);
        static string trim_left(const string &t_data);
        static string trim_right(const string &t_data);
        static string underline(const string &t_data, const char &t_ln_char = '-');
        static string underline(const size_t &t_size, const char &t_ln_char = '-');
        static string upto_first_eol(const string &t_data);
        static string upto_last_eol(const string &t_data);

        static wstring wstr(const string &t_data);

        static string_vector split(const string &t_data, const string &t_delim);

        template<size_t N>
        static string_array<N> split(const string &t_data, const string &t_delim);

        template<LShiftRange R>
        static string_vector str_vector(const R &t_range, const size_t &t_count = 0);

    private:  /* Methods */
        static string_vector split(const string &t_data,
                                   const string &t_delim,
                                   const size_t &t_count);
    };
}

/**
* @brief  Clear the contents of the given range and release its unused memory.
*/
template<scan::ClearableRange R>
inline void scan::Algorithm::clear_and_shrink(R &t_range)
{
    t_range.clear();
    t_range.shrink_to_fit();
}

/**
* @brief  Determine whether the given range is empty.
*/
template<scan::Range R>
inline bool scan::Algorithm::empty(const R &t_range)
{
    return ranges::empty(t_range);
}

/**
* @brief  Determine whether all the given strings contains only integral numbers.
*/
template<scan::StringRange R>
inline bool scan::Algorithm::is_integral(const R &t_range)
{
    return ranges::all_of(t_range, [](const string &l_data) -> bool
    {
        return is_integral(l_data);
    });
}

/**
* @brief  Count the number of matching value_type occurrences in the given range.
*/
template<scan::Range R, class T>
inline size_t scan::Algorithm::count(const R &t_range,
                                     const T &t_value) requires RangeValue<R, T> {

    return static_cast<size_t>(ranges::count(t_range, t_value));
}

/**
* @brief  Calculate the distance between the beginning of the given
*         range and the specified range iterator.
*/
template<scan::Range R, scan::RangeIterator T>
inline size_t scan::Algorithm::distance(const R &t_range, const T &t_iter)
{
    return distance(t_range.begin(), t_iter);
}

/**
* @brief  Calculate the distance between the given range iterators.
*/
template<scan::RangeIterator T>
inline size_t scan::Algorithm::distance(const T &t_beg_it, const T &t_end_it)
{
    size_t offset{ 0 };

    if (t_beg_it != t_end_it)
    {
        offset = static_cast<size_t>(ranges::distance(t_beg_it, t_end_it));
    }
    return offset;
}

/**
* @brief  Convert the given arguments to strings and concatenate the results.
*/
template<scan::LShift ...Args>
inline std::string scan::Algorithm::concat(const Args &...t_args)
{
    sstream stream;
    (stream << ... << t_args);

    return stream.str();
}

/**
* @brief  Interpolate one or more arguments in the given string at the
*         modulus (e.g., %) positions. Modulus literals can be included by
*         prefixing them with back-slashes (e.g., \\%).
*/
template<scan::LShift T, scan::LShift ...Args>
inline std::string scan::Algorithm::fstr(const string &t_msg,
                                         const T &t_arg,
                                         const Args &...t_args) {
    sstream stream;
    stream.precision(fstr_precision);

    // Replace escaped moduli with placeholders
    const string msg{ replace(t_msg, "\\%", "__MOD__") };

    for (const char *p{ &msg[0] }; *p != CHAR_NULL; p++)
    {
        if (*p == '%')
        {
            stream << t_arg;

            if constexpr (sizeof...(t_args) > 0)
            {
                stream << fstr(++p, t_args...);
                break;
            }
            stream << ++p;
            break;
        }
        stream << *p;
    }
    return replace(stream.str(), "__MOD__", "%");
}

/**
* @brief  Join the elements of the given range using the specified delimiter.
*/
template<scan::LShiftRange R>
inline std::string scan::Algorithm::join(const R &t_range, const string &t_delim)
{
    sstream stream;

    for (size_t i{ 0 }; const range_value_t<R> &elem : t_range)
    {
        stream << elem;

        if (i++ != t_range.size() - 1)
        {
            stream << t_delim;
        }
    }
    return stream.str();
}

/**
* @brief  Replace all substring occurrences in the given data with a new substring.
*/
template<scan::StringRange R>
inline std::string scan::Algorithm::replace(const string &t_data,
                                            const R &t_old_subs,
                                            const string &t_new_sub) {
    string new_data{ t_data };

    for (const string &old_sub : t_old_subs)
    {
        boost::replace_all(new_data, old_sub, t_new_sub);
    }
    return new_data;
}

/**
* @brief  Convert the given object to a string using a string stream buffer.
*/
template<scan::LShift T>
inline std::string scan::Algorithm::to_string(const T &t_obj)
{
    sstream stream;
    stream << t_obj;

    return stream.str();
}

/**
* @brief  Split the given data into a fixed-size array using the specified delimiter.
*/
template<size_t N>
inline scan::string_array<N> scan::Algorithm::split(const string &t_data,
                                                    const string &t_delim) {
    static_assert(N > 0 && N < string::npos);

    string_array<N> buffer{ "" };
    const string_vector vect{ split(t_data, t_delim, N) };

    for (size_t i{ 0 }; i < vect.size(); i++)
    {
        if (i >= buffer.size())
        {
            break;
        }
        buffer[i] = vect[i];
    }
    return buffer;
}

/**
* @brief  Convert the given range to a vector of strings.
*/
template<scan::LShiftRange R>
inline scan::string_vector scan::Algorithm::str_vector(const R &t_range,
                                                       const size_t &t_count) {
    string_vector vect;
    const bool count_specified{ t_count > 0 && t_count < t_range.size() };

    // Add elements to vector using a stream buffer
    for (size_t i{ 0 }; const range_value_t<R> &elem : t_range)
    {
        if (i++ >= (count_specified ? t_count : t_range.size()))
        {
            break;
        }
        vect.push_back(to_string(elem));
    }
    return vect;
}

#endif // !ALGORITHM_H
