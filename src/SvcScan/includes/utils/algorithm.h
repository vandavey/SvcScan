/*
* @file
*     algorithm.h
* @brief
*     Header file for range algorithms and utilities.
*/
#pragma once

#ifndef SCAN_ALGORITHM_H
#define SCAN_ALGORITHM_H

#include <sdkddkver.h>
#include <boost/algorithm/string/replace.hpp>
#include "../concepts/concepts.h"
#include "../io/color.h"
#include "expr.h"

namespace scan
{
    /**
    * @brief
    *     Range algorithms and utilities.
    */
    class Algorithm final
    {
    private:  /* Type Aliases */
        using str_iterator = string::const_iterator;
        using wstring      = std::wstring;

    private:  /* Constants */
        static constexpr size_t MOD_HASH = fnv_1a_hash('%');  // Modulus character hash

    public:  /* Constructors & Destructor */
        Algorithm() = delete;
        Algorithm(const Algorithm &) = delete;
        Algorithm(Algorithm &&) = delete;

        virtual ~Algorithm() = default;

    public:  /* Fields */
        static streamsize fstr_precision;  // Format string decimal precision

    private:  /* Fields */
        static string m_fstr_placeholder;  // Format string placeholder

    public:  /* Operators */
        Algorithm &operator=(const Algorithm &) = default;
        Algorithm &operator=(Algorithm &&) = default;

    public:  /* Methods */
        static bool is_integral(const string &t_data, const bool &t_unsigned = false);

        template<StringRange R>
        static bool is_integral(const R &t_range, const bool &t_unsigned = false);

        static bool matches(const string &t_data, const string &t_rgx_pattern);

        static word_t to_word(const string &t_data);
        static uint_t to_uint(const string &t_data);

        template<Range R, class T = range_value_t<R>>
            requires RangeValue<R, T>
        static size_t count(const R &t_range, const T &t_value);

        static size_t count(const string &t_data, const string &t_sub) noexcept;

        template<Range R, RangeIterator T = range_iterator_t<R>>
        static size_t distance(const R &t_range, const T &t_iter);

        template<RangeIterator T>
        static size_t distance(const T &t_beg_iter, const T &t_end_iter);

        static str_iterator find_nth(const string &t_data,
                                     const string &t_sub,
                                     const size_t &t_n,
                                     const bool &t_after = false);

        template<LShift ...ArgsT>
        static string concat(const ArgsT &...t_args);

        static string erase(const string &t_data, const string &t_sub);

        template<LShift T, LShift ...ArgsT>
        static string fstr(const string &t_msg, const T &t_arg, const ArgsT &...t_args);

        template<LShiftRange R>
        static string join(const R &t_range, const string &t_delim);

        template<LShift T, LShift NewT>
        static string replace(const string &t_data,
                              const T &t_old_val,
                              const NewT &t_new_val);

        template<StringRange R = string_vector>
        static string replace(const string &t_data,
                              const R &t_old_subs,
                              const string &t_new_sub);

        static string substr(const string &t_data,
                             const str_iterator &t_beg_it,
                             const str_iterator &t_end_it);

        static string to_lower(const string &t_data);

        template<String T>
        static string to_string(const T &t_obj);

        template<LShiftNonString T>
        static string to_string(const T &t_obj);

        static string to_upper(const string &t_data);
        static string trim_left(const string &t_data);
        static string trim_right(const string &t_data);
        static string underline(const string &t_data, const char &t_ln_char = '-');

        static string underline(const string &t_data,
                                const Color t_color,
                                const char &t_ln_char = '-');

        static string underline(const size_t &t_size, const char &t_ln_char = '-');
        static string upto_first_eol(const string &t_data);
        static string upto_last_eol(const string &t_data);

        static string_vector split(const string &t_data, const string &t_delim);

        template<size_t N>
        static string_array<N> split(const string &t_data, const string &t_delim);

        static string_vector arg_vector(const int &t_argc, char *t_argv[]);

        template<LShiftRange R>
        static string_vector str_vector(const R &t_range, const size_t &t_count = 0);

        template<Range R, SortPredicate F = ranges::less>
            requires Sortable<R, F>
        static R sort(const R &t_range, F t_pred = { });

        template<Range R, class T = range_value_t<R>>
            requires RangeValue<R, T>
        static idx_pairs_t<T> enumerate(const R &t_range, const string &t_filter = { });

    private:  /* Methods */
        static string_vector split(const string &t_data,
                                   const string &t_delim,
                                   const size_t &t_count);
    };
}

/**
* @brief
*     Determine whether all the given strings contain only integral numbers.
*     Optionally consider only unsigned integral numbers as valid.
*/
template<scan::StringRange R>
inline bool scan::Algorithm::is_integral(const R &t_range, const bool &t_unsigned)
{
    return ranges::all_of(t_range, [&t_unsigned](const String auto &l_str) -> bool
    {
        return is_integral(l_str, t_unsigned);
    });
}

/**
* @brief
*     Count the number of matching value type occurrences in the given range.
*/
template<scan::Range R, class T>
    requires scan::RangeValue<R, T>
inline size_t scan::Algorithm::count(const R &t_range, const T &t_value)
{
    return static_cast<size_t>(ranges::count(t_range, t_value));
}

/**
* @brief
*     Calculate the distance between the beginning of
*     the given range and the specified range iterator.
*/
template<scan::Range R, scan::RangeIterator T>
inline size_t scan::Algorithm::distance(const R &t_range, const T &t_iter)
{
    return distance(t_range.begin(), t_iter);
}

/**
* @brief
*     Calculate the distance between the given range iterators.
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
* @brief
*     Convert the given arguments to strings and concatenate the results.
*/
template<scan::LShift ...ArgsT>
inline std::string scan::Algorithm::concat(const ArgsT &...t_args)
{
    static_assert(sizeof...(t_args) > 1);

    sstream stream;
    (stream << ... << t_args);

    return stream.str();
}

/**
* @brief
*     Interpolate one or more arguments in the given string at
*     the modulus (`%`) positions. Modulus literals can be
*     included by prefixing them with back-slashes (`\\%`).
*/
template<scan::LShift T, scan::LShift ...ArgsT>
inline std::string scan::Algorithm::fstr(const string &t_msg,
                                         const T &t_arg,
                                         const ArgsT &...t_args)
{
    sstream stream;
    stream.precision(fstr_precision);

    // Replace escaped moduli with placeholders
    const string msg{ replace(t_msg, "\\%", m_fstr_placeholder) };

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
    return replace(stream.str(), m_fstr_placeholder, "%");
}

/**
* @brief
*     Join the elements of the given range using the specified delimiter.
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
* @brief
*     Replace all substring occurrences in the given data with a new substring.
*/
template<scan::LShift T, scan::LShift NewT>
inline std::string scan::Algorithm::replace(const string &t_data,
                                            const T &t_old_val,
                                            const NewT &t_new_val)
{
    return boost::replace_all_copy(t_data, to_string(t_old_val), to_string(t_new_val));
}

/**
* @brief
*     Replace all substring occurrences in the given data with a new substring.
*/
template<scan::StringRange R>
inline std::string scan::Algorithm::replace(const string &t_data,
                                            const R &t_old_subs,
                                            const string &t_new_sub)
{
    string new_data{ t_data };

    for (const string &old_sub : t_old_subs)
    {
        boost::replace_all(new_data, old_sub, t_new_sub);
    }
    return new_data;
}

/**
* @brief
*     Statically cast the given string-like object to a string.
*/
template<scan::String T>
inline std::string scan::Algorithm::to_string(const T &t_obj)
{
    return static_cast<string>(t_obj);
}

/**
* @brief
*     Convert the given object to a string using a string stream.
*/
template<scan::LShiftNonString T>
inline std::string scan::Algorithm::to_string(const T &t_obj)
{
    sstream stream;
    stream << t_obj;

    return stream.str();
}

/**
* @brief
*     Split the given data into a fixed-size array using the specified delimiter.
*/
template<size_t N>
inline scan::string_array<N> scan::Algorithm::split(const string &t_data,
                                                    const string &t_delim)
{
    static_assert(N > 0 && N < string::npos);

    string_array<N> buffer;
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
* @brief
*     Convert each element in the given range to a string
*     and add the results to a new string vector.
*/
template<scan::LShiftRange R>
inline scan::string_vector scan::Algorithm::str_vector(const R &t_range,
                                                       const size_t &t_count)
{
    string_vector vect;
    const bool count_specified{ t_count > 0 && t_count < t_range.size() };

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

/**
* @brief
*     Sort the given range using the specified comparison predicate.
*/
template<scan::Range R, scan::SortPredicate F>
    requires scan::Sortable<R, F>
inline R scan::Algorithm::sort(const R &t_range, F t_pred)
{
    R buffer{ t_range };
    ranges::sort(buffer, t_pred);

    return buffer;
}

/**
* @brief
*     Enumerate the values of the given range as a vector of index-element pairs
*     where the element value matches the specified regex filter pattern.
*/
template<scan::Range R, class T>
    requires scan::RangeValue<R, T>
inline scan::idx_pairs_t<T> scan::Algorithm::enumerate(const R &t_range,
                                                       const string &t_filter)
{
    idx_pairs_t<T> unfiltered_pairs;

    // Enumerate all range values
    for (size_t i{ 0 }; i < t_range.size(); i++)
    {
        unfiltered_pairs.push_back({ i, t_range[i] });
    }
    idx_pairs_t<T> filtered_pairs;

    // Filter the enumerated results using regex
    if (!t_filter.empty())
    {
        for (const IndexPair<T> &pair : unfiltered_pairs)
        {
            if (matches(pair.value, t_filter))
            {
                filtered_pairs.push_back(pair);
            }
        }
    }
    return t_filter.empty() ? unfiltered_pairs : filtered_pairs;
}

#endif // !SCAN_ALGORITHM_H
