/*
* @file
*     algo.h
* @brief
*     Header file for range algorithms and utilities.
*/
#pragma once

#ifndef SCAN_ALGO_H
#define SCAN_ALGO_H

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <string>
#include "../concepts/concepts.h"
#include "../containers/generic/index_pair.h"
#include "../io/color.h"
#include "alias.h"
#include "c_string.h"
#include "const_defs.h"

/**
* @brief
*     Range algorithms and utilities.
*/
namespace scan::algo
{
    /**
    * @brief
    *     Hash a byte array using hash algorithm FNV-1a.
    */
    template<HashableByte T>
    consteval size_t fnv_1a_hash(const T *t_bytes_ptr, const size_t &t_count = 1U)
        noexcept
    {
        size_t hash{ FNV_OFFSET_BASIS };

        for (size_t i{ 0U }; i < sizeof(T) * t_count; i++)
        {
            hash ^= static_cast<size_t>(t_bytes_ptr[i]);
            hash *= FNV_PRIME;
        }
        return hash;
    }

    /**
    * @brief
    *     Range algorithm constant fields.
    */
    inline namespace defs
    {
        /// @brief  String decimal point precision.
        constexpr streamsize PRECISION = 4;

        /// @brief  String trimming characters.
        constexpr cstr_t TRIM_CHARS = "\f\n\r\t\v ";

        /// @brief  Format string placeholder wrapper.
        constexpr CString<~fnv_1a_hash(MOD)> FSTR_PLACEHOLDER = {};
    }

    /**
    * @brief
    *     Count the number of matching value type occurrences in the given range.
    */
    template<Range R = string_vector, class T = range_value_t<R>>
        requires RangeValue<R, T>
    constexpr size_t count(const R &t_range, const T &t_value)
    {
        return static_cast<size_t>(ranges::count(t_range, t_value));
    }

    /**
    * @brief
    *     Count the number of substring occurrences in the given data.
    */
    constexpr size_t count(const string &t_data, const string &t_sub) noexcept
    {
        size_t count{ 0U };
        size_t offset{ 0U };

        size_t i;

        while ((i = t_data.find(t_sub, offset)) != string::npos)
        {
            offset = t_data.find(t_sub, i + t_sub.size());
            count++;
        }
        return count;
    }

    /**
    * @brief
    *     Determine whether an unsigned integral sum causes an integer overflow.
    */
    template<std::unsigned_integral T, std::unsigned_integral U>
    constexpr bool sum_overflow(const T &t_lhs_num, const U &t_rhs_num) noexcept
    {
        return t_lhs_num + static_cast<T>(t_rhs_num) < t_lhs_num;
    }

    /**
    * @brief
    *     Find the location of the n-th substring occurrence in the given data.
    */
    constexpr size_t find_nth(const string &t_data,
                              const string &t_sub,
                              const size_t &t_n,
                              const bool &t_after_sub = false)
        noexcept
    {
        size_t count{ 0U };
        size_t offset{ 0U };

        size_t i;
        size_t match_offset{ string::npos };

        while ((i = t_data.find(t_sub, offset)) != string::npos)
        {
            if (++count == t_n)
            {
                match_offset = i;
                break;
            }
            offset = t_data.find(t_sub, i + t_sub.size());
        }

        // Offset match position to exclude substring
        if (t_after_sub && !sum_overflow(match_offset, t_sub.size()))
        {
            match_offset += t_sub.size();
        }
        return match_offset;
    }

    /**
    * @brief
    *     Get the absolute value of the given signed integral value.
    */
    template<std::integral T>
    constexpr T abs(const T &t_num) noexcept
    {
        return t_num >= 0 ? t_num : -t_num;
    }

    /**
    * @brief
    *     Statically cast the given integral value to a string.
    */
    template<size_t N>
    constexpr string to_string() noexcept
    {
        return static_cast<string>(CString<N>());
    }

    /**
    * @brief
    *     Statically cast the given object to a string.
    */
    template<LShift T>
    constexpr string to_string(const T &t_obj) noexcept(String<T>)
    {
        string result;

        if constexpr (String<T>)
        {
            result = static_cast<string>(t_obj);
        }
        else  // Constant conversion unsupported
        {
            sstream stream;
            stream.precision(PRECISION);

            stream << t_obj;
            result = stream.str();
        }
        return result;
    }

    /**
    * @brief
    *     Convert the given arguments to strings and concatenate the results.
    */
    template<LShift ...ArgsT>
    constexpr string concat(const ArgsT &...t_args) noexcept((String<ArgsT> && ...))
        requires(sizeof...(t_args) > 1)
    {
        return (to_string(t_args) + ...);
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    template<String T, String NewT>
    constexpr string replace(const string &t_data, const T &t_old, const NewT &t_new)
        noexcept
    {
        const string old_sub{ static_cast<string>(t_old) };
        const string new_sub{ static_cast<string>(t_new) };

        size_t i{ 0U };
        string result{ t_data };

        while ((i = result.find(old_sub, i)) != string::npos)
        {
            result.replace(i, old_sub.size(), new_sub);
            i += new_sub.size();
        }
        return result;
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    template<StringRange R = string_vector, String T>
    constexpr string replace(const string &t_data,
                             const R &t_old_subs,
                             const T &t_new)
        noexcept
    {
        string new_data{ t_data };

        for (const string &old_sub : t_old_subs)
        {
            new_data = replace(new_data, old_sub, t_new);
        }
        return new_data;
    }

    /**
    * @brief
    *     Erase all occurrences of the specified substring from the given data.
    */
    constexpr string erase(const string &t_data, const string &t_sub)
    {
        return replace(t_data, t_sub, "");
    }

    /**
    * @brief
    *     Interpolate one or more arguments in the given string at
    *     the modulus (`%`) positions. Modulus literals can be
    *     included by prefixing them with back-slashes (`\\%`).
    */
    template<LShift T, LShift ...ArgsT>
    constexpr string fstr(const string &t_msg, const T &t_arg, const ArgsT &...t_args)
    {
        // Replace escaped moduli with placeholders
        const string msg{ replace(t_msg, concat("\\", MOD), FSTR_PLACEHOLDER.data()) };

        string fmt_msg;

        for (const char *p{ &msg[0] }; *p != CHAR_NULL; p++)
        {
            if (*p == *MOD)
            {
                fmt_msg += to_string(t_arg);

                if constexpr (sizeof...(t_args) > 0)
                {
                    fmt_msg += fstr(++p, to_string(t_args)...);
                    break;
                }
                fmt_msg += ++p;
                break;
            }
            fmt_msg += *p;
        }
        return replace(fmt_msg, FSTR_PLACEHOLDER.data(), MOD);
    }

    /**
    * @brief
    *     Join the elements of the given range using the specified delimiter.
    */
    template<LShiftRange R>
    constexpr string join(const R &t_range, const string &t_delim)
    {
        string result;

        for (RangeIterator auto it{ t_range.begin() }; it != t_range.end(); ++it)
        {
            result += to_string(*it);

            if (it + 1 != t_range.end())
            {
                result += t_delim;
            }
        }
        return result;
    }

    /**
    * @brief
    *     Remove all leading whitespace characters from the given data.
    */
    constexpr string trim_left(const string &t_data)
    {
        string data{ t_data };
        data.erase(0U, data.find_first_not_of(TRIM_CHARS));
        return data;
    }

    /**
    * @brief
    *     Remove all trailing whitespace characters from the given data.
    */
    constexpr string trim_right(const string &t_data)
    {
        string data{ t_data };
        data.erase(data.find_last_not_of(TRIM_CHARS) + 1U);
        return data;
    }

    /**
    * @brief
    *     Create an underline whose size is equal to the given string size.
    */
    constexpr string underline(const size_t &t_size, const char &t_ln_char = CHAR_DASH)
    {
        return string(t_size, t_ln_char == CHAR_NULL ? CHAR_DASH : t_ln_char);
    }

    /**
    * @brief
    *     Read the given string data until the first EOL sequence is detected.
    */
    constexpr string up_to_first_eol(const string &t_data)
    {
        string buffer{ t_data };

        if (!t_data.empty())
        {
            size_t offset{ t_data.find(CRLF) };

            if (offset != string::npos)
            {
                buffer = t_data.substr(0U, offset);
            }
            else if ((offset = t_data.find(LF)) != string::npos)
            {
                buffer = t_data.substr(0U, offset);
            }
        }
        return buffer;
    }

    /**
    * @brief
    *     Read the given string data until the last EOL sequence is detected.
    */
    constexpr string up_to_last_eol(const string &t_data)
    {
        string buffer{ t_data };

        if (!t_data.empty())
        {
            size_t offset{ t_data.rfind(CRLF) };

            if (offset != string::npos)
            {
                buffer = t_data.substr(0U, offset);
            }
            else if ((offset = t_data.rfind(LF)) != string::npos)
            {
                buffer = t_data.substr(0U, offset);
            }
        }
        return buffer;
    }

    /**
    * @brief
    *     Split the given data using the specified delimiter into a vector
    *     whose size is less than or equal to the specified element count.
    */
    constexpr string_vector split(const string &t_data,
                                  const string &t_delim,
                                  const size_t &t_count = string::npos)
    {
        string_vector vect;

        if (t_count > 0 && !t_delim.empty() && t_data.find(t_delim) != string::npos)
        {
            size_t offset{ 0U };
            size_t split_count{ 0U };

            size_t i;

            while ((i = t_data.find_first_not_of(t_delim, offset)) != string::npos)
            {
                if (++split_count == t_count)
                {
                    vect.push_back(t_data.substr(i));
                    break;
                }

                offset = t_data.find(t_delim, i);
                vect.push_back(t_data.substr(i, offset - i));
            }
        }
        else if (!t_data.empty())
        {
            vect.push_back(t_data);
        }

        return vect;
    }

    /**
    * @brief
    *     Split the given data into a fixed-size array using the specified delimiter.
    */
    template<size_t N>
    constexpr string_array<N> split(const string &t_data, const string &t_delim)
        requires(N > 0 && N < string::npos)
    {
        string_array<N> buffer;
        const string_vector vect{ split(t_data, t_delim, N) };

        for (size_t i{ 0U }; i < vect.size(); i++)
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
    *     Initialize a new string vector from the given command-line arguments.
    */
    constexpr string_vector arg_vector(const int &t_argc, char **t_argv)
    {
        string_vector arg_vect;

        if (t_argc > 1 && t_argv != nullptr)
        {
            for (int i{ 1 }; i < t_argc; i++)
            {
                if (t_argv[i] != nullptr)
                {
                    arg_vect.push_back(t_argv[i]);
                }
            }
        }
        return arg_vect;
    }

    /**
    * @brief
    *     Convert each element in the given range to a string
    *     and add the results to a new string vector.
    */
    template<LShiftRange R = string_vector>
    constexpr string_vector str_vector(const R &t_range,
                                       const size_t &t_count = string::npos)
    {
        string_vector vect;

        const size_t count{ t_count > 0 ? t_count : string::npos };

        for (size_t i{ 0U }; i < t_range.size() && i < count; i++)
        {
            vect.push_back(to_string(t_range[i]));
        }
        return vect;
    }

    /**
    * @brief
    *     Sort the given range using the specified comparison predicate.
    */
    template<Range R = string_vector, SortPredicate F = ranges::less>
        requires Sortable<R, F>
    constexpr R sort(const R &t_range, F t_pred = {})
    {
        R buffer{ t_range };
        ranges::sort(buffer, t_pred);
        return buffer;
    }

    bool is_integral(const string &t_data, const bool &t_unsigned = false);

    template<StringRange R = string_vector>
    bool is_integral(const R &t_range, const bool &t_unsigned = false);

    bool matches(const string &t_data, const string &t_rgx_pattern);

    uint16_t to_word(const string &t_data);
    uint_t to_uint(const string &t_data);

    string to_lower(const string &t_data);
    string to_upper(const string &t_data);
    string underline(const string &t_data, const char &t_ln_char = CHAR_DASH);

    string underline(const string &t_data,
                     const Color t_color,
                     const char &t_ln_char = CHAR_DASH);

    template<Range R = string_vector, class T = range_value_t<R>>
        requires RangeValue<R, T>
    idx_pairs_t<T> enumerate(const R &t_range, const string &t_filter = {});
}

/**
* @brief
*     Determine whether all the given strings contain only integral numbers.
*     Optionally consider only unsigned integral numbers as valid.
*/
template<scan::StringRange R>
inline bool scan::algo::is_integral(const R &t_range, const bool &t_unsigned)
{
    return ranges::all_of(t_range, [&t_unsigned](const String auto &l_str) -> bool
    {
        return is_integral(l_str, t_unsigned);
    });
}

/**
* @brief
*     Enumerate the values of the given range as a vector of index-element pairs
*     where the element value matches the specified regex filter pattern.
*/
template<scan::Range R, class T>
    requires scan::RangeValue<R, T>
inline scan::idx_pairs_t<T> scan::algo::enumerate(const R &t_range,
                                                  const string &t_filter)
{
    idx_pairs_t<T> unfiltered_pairs;

    // Enumerate all range values
    for (size_t i{ 0U }; i < t_range.size(); i++)
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

#endif // !SCAN_ALGO_H
