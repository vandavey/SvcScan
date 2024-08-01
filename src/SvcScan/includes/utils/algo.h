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
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include "../concepts/concepts.h"
#include "../io/color.h"
#include "aliases.h"
#include "c_string.h"
#include "const_defs.h"
#include "indexed_arg.h"
#include "literals.h"

/**
* @brief
*     Range algorithms and utilities.
*/
namespace scan::algo
{
    /**
    * @brief
    *     Hash a byte using hash algorithm FNV-1A.
    */
    template<HashableByte T>
    consteval size_t fnv_1a_hash(T t_byte) noexcept
    {
        return (FNV_OFFSET_BASIS ^ static_cast<size_t>(t_byte)) * FNV_PRIME;
    }

    /**
    * @brief
    *     Range algorithm constant fields.
    */
    inline namespace defs
    {
        /// @brief  String decimal point precision.
        constexpr streamsize PRECISION = 4_i64;

        /// @brief  String trimming characters.
        constexpr cstr_t TRIM_CHARS = "\f\n\r\t\v ";

        /// @brief  Format string placeholder wrapper.
        constexpr CString<~fnv_1a_hash(*MOD)> FSTR_PLACEHOLDER = {};
    }

    /**
    * @brief
    *     Get the string representation of the given integral value.
    */
    template<size_t N>
    constexpr string to_string() noexcept
    {
        return static_cast<string>(CString<N>());
    }

    /**
    * @brief
    *     Get the string representation of the given value.
    */
    template<LShift T>
    constexpr string to_string(const T& t_value) noexcept(String<T>)
    {
        string result;

        if constexpr (String<T>)
        {
            result = static_cast<string>(t_value);
        }
        else  // Constant conversion unsupported
        {
            sstream stream;
            stream.precision(PRECISION);

            stream << t_value;
            result = stream.str();
        }
        return result;
    }

    /**
    * @brief
    *     Get the string representation of the given value.
    */
    template<LShift T>
    constexpr string to_string(T&& t_value) noexcept(String<T>)
    {
        string result;

        if constexpr (String<T>)
        {
            result = static_cast<string>(std::forward<T>(t_value));
        }
        else  // Constant conversion unsupported
        {
            sstream stream;
            stream.precision(PRECISION);

            stream << std::forward<T>(t_value);
            result = stream.str();
        }
        return result;
    }

    /**
    * @brief
    *     Determine whether a given value is equal to any
    *     of the other specified values which follow it.
    */
    template<class T, class... ArgsT>
        requires AllComparable<T, ArgsT...>
    constexpr bool any_equal(const T& t_arg, const ArgsT&... t_args)
    {
        bool equal;

        if constexpr (AllStrings<T, ArgsT...>)
        {
            equal = ((to_string(t_arg) == t_args) || ...);
        }
        else
        {
            equal = ((t_arg == t_args) || ...);
        }
        return equal;
    }

    /**
    * @brief
    *     Determine whether an unsigned integral sum causes an integer overflow.
    */
    template<std::unsigned_integral T, std::unsigned_integral U>
    constexpr bool sum_overflow(T t_lhs_num, U t_rhs_num) noexcept
    {
        return t_lhs_num + static_cast<T>(t_rhs_num) < t_lhs_num;
    }

    /**
    * @brief
    *     Count the number of matching value type occurrences in the given range.
    */
    template<Range R = string_vector, class T = range_value_t<R>>
        requires RangeValue<R, T>
    constexpr size_t count(const R& t_range, const T& t_value)
    {
        return static_cast<size_t>(ranges::count(t_range, t_value));
    }

    /**
    * @brief
    *     Count the number of substring occurrences in the given data.
    */
    constexpr size_t count(const string& t_data, const string& t_sub) noexcept
    {
        size_t count{0_st};
        size_t offset{0_st};

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
    *     Get the current maximum key size from the given map.
    */
    template<StringMap M>
    constexpr size_t max_key_size(const M& t_map)
    {
        size_t max_size{0_st};

        for (size_t size; const StringPair auto& pair : t_map)
        {
            if ((size = pair.first.size()) > max_size)
            {
                max_size = size;
            }
        }
        return max_size;
    }

    /**
    * @brief
    *     Get the current maximum key size from the given maps.
    */
    template<StringMap... ArgsT>
    constexpr size_t max_key_size(const ArgsT&... t_maps)
    {
        size_t max_size{0_st};

        for (size_t size; const auto& map : {t_maps...})
        {
            if ((size = max_key_size(map)) > max_size)
            {
                max_size = size;
            }
        }
        return max_size;
    }

    /**
    * @brief
    *     Find the location of the n-th substring occurrence in the given data.
    */
    constexpr size_t find_nth(const string& t_data,
                              const string& t_sub,
                              size_t t_n,
                              bool t_after_sub = false)
        noexcept
    {
        size_t count{0_st};
        size_t offset{0_st};

        size_t i;
        size_t match_offset{string::npos};

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
    *     Get the absolute value of the given integral value.
    */
    template<std::integral T>
    constexpr T abs(T t_num) noexcept
    {
        T abs_value{t_num};

        if constexpr (!std::unsigned_integral<T>)
        {
            abs_value = abs_value >= 0 ? abs_value : -abs_value;
        }
        return abs_value;
    }

    /**
    * @brief
    *     Get the maximum value from the given integral numbers.
    */
    template<std::integral... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr std::integral auto maximum(const ArgsT&... t_nums)
    {
        return (std::max)({t_nums...});
    }

    /**
    * @brief
    *     Get the maximum value from the given integral numbers.
    */
    template<std::integral... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr std::integral auto maximum(ArgsT&&... t_nums)
    {
        return (std::max)({std::forward<ArgsT>(t_nums)...});
    }

    /**
    * @brief
    *     Get the minimum value from the given integral numbers.
    */
    template<std::integral... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr std::integral auto minimum(const ArgsT&... t_nums)
    {
        return (std::min)({t_nums...});
    }

    /**
    * @brief
    *     Get the minimum value from the given integral numbers.
    */
    template<std::integral... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr std::integral auto minimum(ArgsT&&... t_nums)
    {
        return (std::min)({std::forward<ArgsT>(t_nums)...});
    }

    /**
    * @brief
    *     Convert the given arguments to strings and concatenate the results.
    */
    template<LShift... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr string concat(const ArgsT&... t_args)
    {
        return (to_string(t_args) + ...);
    }

    /**
    * @brief
    *     Convert the given arguments to strings and concatenate the results.
    */
    template<LShift... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr string concat(ArgsT&&... t_args)
    {
        return (to_string(std::forward<ArgsT>(t_args)) + ...);
    }

    /**
    * @brief
    *     Appending whitespace padding to the given data so
    *     the resulting size matches the specified result size.
    */
    constexpr string pad(const string& t_data, size_t t_result_size)
    {
        string padded_data{t_data};
        const size_t delta{t_result_size - t_data.size()};

        if (delta > 0 && delta != NPOS)
        {
            padded_data += string(delta, ' ');
        }
        return padded_data;
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    template<String T, String NewT>
    constexpr string replace(const string& t_data, const T& t_old, const NewT& t_new)
        noexcept
    {
        const string old_sub{static_cast<string>(t_old)};
        const string new_sub{static_cast<string>(t_new)};

        size_t i{0_st};
        string result{t_data};

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
    constexpr string replace(const string& t_data,
                             const R& t_old_subs,
                             const T& t_new)
        noexcept
    {
        string new_data{t_data};

        for (const string& old_sub : t_old_subs)
        {
            new_data = replace(new_data, old_sub, t_new);
        }
        return new_data;
    }

    /**
    * @brief
    *     Erase all occurrences of the specified substring from the given data.
    */
    constexpr string erase(const string& t_data, const string& t_sub)
    {
        return replace(t_data, t_sub, "");
    }

    /**
    * @brief
    *     Interpolate one or more arguments in the given string at
    *     the modulus (`%`) positions. Modulus literals can be
    *     included by prefixing them with back-slashes (`\\%`).
    */
    template<LShift T, LShift... ArgsT>
    constexpr string fstr(const string& t_msg, const T& t_arg, const ArgsT&... t_args)
    {
        // Replace escaped moduli with placeholders
        const string msg{replace(t_msg, concat("\\", MOD), FSTR_PLACEHOLDER.data())};

        string fmt_msg;

        for (const char* p{&msg[0]}; *p != CHAR_NULL; p++)
        {
            if (*p == *MOD)
            {
                fmt_msg += to_string(t_arg);

                if constexpr (AtLeastOneParam<ArgsT...>)
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
    *     Join the values of the given range using the specified delimiter.
    */
    template<LShiftRange R = string_vector>
    constexpr string join(const R& t_range, const string& t_delim)
    {
        string result;

        for (RangeIterator auto it{t_range.begin()}; it != t_range.end(); ++it)
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
    *     Join the values of the given range using a line-feed delimiter.
    */
    template<LShiftRange R = string_vector>
    constexpr string join_lines(const R& t_range)
    {
        return join(t_range, LF);
    }

    /**
    * @brief
    *     Remove all leading whitespace characters from the given data.
    */
    constexpr string trim_left(const string& t_data)
    {
        string data{t_data};
        data.erase(0_st, data.find_first_not_of(TRIM_CHARS));
        return data;
    }

    /**
    * @brief
    *     Remove all trailing whitespace characters from the given data.
    */
    constexpr string trim_right(const string& t_data)
    {
        string data{t_data};
        data.erase(data.find_last_not_of(TRIM_CHARS) + 1_st);
        return data;
    }

    /**
    * @brief
    *     Create an underline whose size is equal to the given string size.
    */
    constexpr string underline(size_t t_size, char t_ln_char = CHAR_DASH)
    {
        return string(t_size, t_ln_char == CHAR_NULL ? CHAR_DASH : t_ln_char);
    }

    /**
    * @brief
    *     Read the given string data until the first occurrence
    *     of any of the specified substrings is found.
    */
    template<String... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr string up_to_first(const string& t_data, const ArgsT&... t_args)
    {
        string buffer;
        size_t offset{string::npos};

        for (const String auto& sub : {t_args...})
        {
            if ((offset = t_data.find(sub)) != string::npos)
            {
                buffer = t_data.substr(0_st, offset);
                break;
            }
        }
        return offset == string::npos ? t_data : buffer;
    }

    /**
    * @brief
    *     Read the given string data until the first EOL substring is found.
    */
    constexpr string up_to_first_eol(const string& t_data)
    {
        return up_to_first(t_data, CRLF, LF);
    }

    /**
    * @brief
    *     Read the given string data until the last occurrence
    *     of any of the specified substrings is found.
    */
    template<String... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    constexpr string up_to_last(const string& t_data, const ArgsT&... t_args)
    {
        string buffer;
        size_t offset{string::npos};

        for (const String auto& sub : {t_args...})
        {
            if ((offset = t_data.rfind(sub)) != string::npos)
            {
                buffer = t_data.substr(0_st, offset);
                break;
            }
        }
        return offset == string::npos ? t_data : buffer;
    }

    /**
    * @brief
    *     Read the given string data until the last EOL substring is found.
    */
    constexpr string up_to_last_eol(const string& t_data)
    {
        return up_to_last(t_data, CRLF, LF);
    }

    /**
    * @brief
    *     Create a vector inclusively containing all integers within
    *     the given range bounds. The maximum and minimum values will be
    *     swapped when the maximum value is less than the minimum value.
    */
    template<std::integral T>
    constexpr vector<T> iota(T t_min, T t_max)
    {
        const T min{minimum(t_min, t_max)};
        const T max{maximum(t_min, t_max)};

        vector<T> buffer(static_cast<size_t>(max - min) + 1_st);
        std::iota(buffer.begin(), buffer.end(), min);

        return buffer;
    }

    /**
    * @brief
    *     Split the given data using the specified delimiter into a vector
    *     whose size is less than or equal to the specified value count.
    */
    constexpr string_vector split(const string& t_data,
                                  const string& t_delim,
                                  size_t t_count = string::npos)
    {
        string_vector vect;

        if (t_count > 0 && !t_delim.empty() && t_data.find(t_delim) != string::npos)
        {
            size_t offset{0_st};
            size_t split_count{0_st};

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
    constexpr string_array<N> split(const string& t_data, const string& t_delim)
        requires(N > 0 && N < string::npos)
    {
        string_array<N> buffer;
        const string_vector vect{split(t_data, t_delim, N)};

        for (size_t i{0_st}; i < vect.size(); i++)
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
    constexpr string_vector arg_vector(int t_argc, char* t_argv[])
    {
        string_vector arg_vect;

        if (t_argc > 1 && t_argv != nullptr)
        {
            for (int i{1}; i < t_argc; i++)
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
    *     Convert each value in the given range to a string
    *     and add the results to a new string vector.
    */
    template<LShiftRange R = string_vector>
    constexpr string_vector str_vector(const R& t_range, size_t t_count = string::npos)
    {
        string_vector vect;
        const size_t count{t_count > 0 ? t_count : string::npos};

        for (size_t i{0_st}; i < t_range.size() && i < count; i++)
        {
            vect.push_back(to_string(t_range[i]));
        }
        return vect;
    }

    /**
    * @brief
    *     Sort the given range using the specified comparison predicate and projection.
    */
    template<Range R, class F = ranges::less, class P = std::identity>
        requires SortableRange<R, F, P>
    constexpr R sort(const R& t_range, F t_sort_pred = {}, P t_proj_func = {})
    {
        R buffer{t_range};
        ranges::sort(buffer, t_sort_pred, t_proj_func);
        return buffer;
    }

    /**
    * @brief
    *     Sort the given range using the specified comparison predicate and projection.
    */
    template<Range R, class F = ranges::less, class P = std::identity>
        requires SortableRange<R, F, P>
    constexpr R& sort(R& t_range, F t_sort_pred = {}, P t_proj_func = {})
    {
        ranges::sort(t_range, t_sort_pred, t_proj_func);
        return t_range;
    }

    /**
    * @brief
    *     Normalize the size of the keys in the given map by appending whitespace padding.
    */
    template<StringMap M>
    constexpr M pad_keys(const M& t_map, size_t t_field_size)
    {
        M map{};

        for (const StringPair auto& pair : t_map)
        {
            map[pad(pair.first, t_field_size)] = pair.second;
        }
        return map;
    }

    bool is_integral(const string& t_data, bool t_unsigned = false);

    template<StringRange R = string_vector>
    bool is_integral(const R& t_range, bool t_unsigned = false);

    bool matches(const string& t_data, const string& t_rgx_pattern);

    uint16_t to_word(const string& t_data);
    uint_t to_uint(const string& t_data);

    string to_lower(const string& t_data);
    string to_upper(const string& t_data);
    string underline(const string& t_data, char t_ln_char = CHAR_DASH);
    string underline(const string& t_data, Color t_color, char t_ln_char = CHAR_DASH);

    template<Range R = string_vector, class T = range_value_t<R>>
        requires RangeValue<R, T>
    vector<IndexedArg> enumerate(const R& t_range, const string& t_filter = {});
}

/**
* @brief
*     Determine whether all the given strings contain only integral numbers.
*     Optionally consider only unsigned integral numbers as valid.
*/
template<scan::StringRange R>
inline bool scan::algo::is_integral(const R& t_range, bool t_unsigned)
{
    return ranges::all_of(t_range, [&t_unsigned](const String auto& l_str) -> bool
    {
        return is_integral(l_str, t_unsigned);
    });
}

/**
* @brief
*     Enumerate the values of the given range as a vector of indexed
*     command-line arguments. Only values matching the specified regex
*     pattern will be enumerated when a filter pattern is provided.
*/
template<scan::Range R, class T>
    requires scan::RangeValue<R, T>
inline std::vector<scan::IndexedArg> scan::algo::enumerate(const R& t_range,
                                                           const string& t_filter)
{
    vector<IndexedArg> indexed_args;

    // Enumerate all range values
    for (size_t i{0_st}; i < t_range.size(); i++)
    {
        if (t_filter.empty() || matches(t_range[i], t_filter))
        {
            indexed_args.push_back(IndexedArg{i, t_range[i]});
        }
    }
    return indexed_args;
}

#endif // !SCAN_ALGO_H
