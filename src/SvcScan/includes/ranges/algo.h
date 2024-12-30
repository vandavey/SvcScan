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
#include "../console/color.h"
#include "../console/indexed_arg.h"
#include "../utils/aliases.h"
#include "../utils/c_string.h"
#include "../utils/const_defs.h"
#include "../utils/eol.h"
#include "../utils/literals.h"

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
        /// @brief  Default wrapped line size.
        constexpr size_t LN_SIZE_DEFAULT = 95_u16;

        /// @brief  Minimum wrapped line size.
        constexpr size_t LN_SIZE_MIN = 50_u16;

        /// @brief  String decimal point precision.
        constexpr streamsize PRECISION = 4_i64;

        /// @brief  ANSI control sequence introducer.
        constexpr c_string_t CSI = "\x1b[";

        /// @brief  String trimming characters.
        constexpr c_string_t TRIM_CHARS = "\t\n\v\f\r ";

        /// @brief  String wrapping delimiter characters.
        constexpr c_string_t WRAP_CHARS = "\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~";

        /// @brief  Modulus string placeholder wrapper.
        constexpr CString<~fnv_1a_hash(*MOD)> MOD_PLACEHOLDER = {};
    }

    /**
    * @brief
    *     Determine whether the given size type offset
    *     is equal to its maximum possible value.
    */
    constexpr bool is_npos(size_t t_offset) noexcept
    {
        return t_offset == NPOS;
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    constexpr string& replace(string& t_data,
                              const string& t_old_sub,
                              const string& t_new_sub)
    {
        if (!t_data.empty() && t_old_sub != t_new_sub)
        {
            size_t index{0_st};

            while (!is_npos(index = t_data.find(t_old_sub, index)))
            {
                t_data.replace(index, t_old_sub.size(), t_new_sub);
                index += t_new_sub.size();
            }
        }
        return t_data;
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    constexpr string replace(const string& t_data,
                             const string& t_old_sub,
                             const string& t_new_sub)
    {
        string buffer{t_data};
        return replace(buffer, t_old_sub, t_new_sub);
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    template<StringRange R = vector<string>>
    constexpr string& replace(string& t_data,
                              const R& t_old_subs,
                              const string& t_new_sub)
    {
        if (!t_data.empty())
        {
            for (const string& old_sub : t_old_subs)
            {
                replace(t_data, old_sub, t_new_sub);
            }
        }
        return t_data;
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    template<StringRange R = vector<string>>
    constexpr string replace(const string& t_data,
                             const R& t_old_subs,
                             const string& t_new_sub)
    {
        string buffer{t_data};
        return replace(buffer, t_old_subs, t_new_sub);
    }

    /**
    * @brief
    *     Get the string representation of the given integral value.
    */
    template<size_t N>
    constexpr string to_string() noexcept
    {
        return static_cast<string>(CString<N>{});
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
    *     Determine whether the given range contains the specified value.
    */
    template<Range R, class T = range_value_t<R>>
        requires RangeValue<R, T>
    constexpr bool contains(const R& t_range, const T& t_value) noexcept
    {
        return ranges::find(t_range, t_value) != t_range.end();
    }

    /**
    * @brief
    *     Determine whether the given range contains the specified value.
    */
    template<Range R, class T = range_value_t<R>>
        requires RangeValue<R, T>
    constexpr bool contains(const R& t_range, T&& t_value) noexcept
    {
        return ranges::find(t_range, std::forward<T>(t_value)) != t_range.end();
    }

    /**
    * @brief
    *     Determine whether an unsigned integral sum causes an integer overflow.
    */
    template<std::unsigned_integral T, std::unsigned_integral T2>
    constexpr bool sum_overflow(T t_lhs_num, T2 t_rhs_num) noexcept
    {
        return t_lhs_num + static_cast<T>(t_rhs_num) < t_lhs_num;
    }

    /**
    * @brief
    *     Determine whether the given size type offset is valid and greater than zero.
    */
    constexpr bool valid_offset(size_t t_offset) noexcept
    {
        return !is_npos(t_offset) && t_offset > 0;
    }

    /**
    * @brief
    *     Count the number of matching value type occurrences in the given range.
    */
    template<Range R = vector<string>, class T = range_value_t<R>>
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

        size_t index;

        while (!is_npos(index = t_data.find(t_sub, offset)))
        {
            offset = t_data.find(t_sub, index + t_sub.size());
            count++;
        }
        return count;
    }

    /**
    * @brief
    *     Find the index of the n-th substring occurrence in the given data.
    */
    constexpr size_t find_nth(const string& t_data,
                              const string& t_sub,
                              size_t t_n,
                              bool t_after_sub = false)
        noexcept
    {
        size_t count{0_st};
        size_t offset{0_st};

        size_t index;
        size_t match_offset{string::npos};

        while (!is_npos(index = t_data.find(t_sub, offset)))
        {
            if (++count == t_n)
            {
                match_offset = index;
                break;
            }
            offset = t_data.find(t_sub, index + t_sub.size());
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
    *     Get the size type offset of leading whitespace in the given string.
    */
    constexpr size_t indent_offset(const string& t_data)
    {
        return t_data.find_first_not_of(' ');
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
    *     Find the first matching value in the given range.
    */
    template<class R, class T = range_value_t<R>>
        requires RangeValue<R, T>
    constexpr typename R::const_iterator find(const R& t_range, const T& t_value)
    {
        return {ranges::find(t_range, t_value)};
    }

    /**
    * @brief
    *     Find the first matching value in the given range.
    */
    template<class R, class T = range_value_t<R>>
        requires RangeValue<R, T>
    constexpr typename R::const_iterator find(const R& t_range, T&& t_value)
    {
        return {ranges::find(t_range, std::forward<T>(t_value))};
    }

    /**
    * @brief
    *     Convert the given arguments to strings and concatenate the results.
    */
    template<LShift... ArgsT>
        requires AtLeastTwoParams<ArgsT...>
    constexpr string concat(const ArgsT&... t_args)
    {
        return (to_string(t_args) + ...);
    }

    /**
    * @brief
    *     Convert the given arguments to strings and concatenate the results.
    */
    template<LShift... ArgsT>
        requires AtLeastTwoParams<ArgsT...>
    constexpr string concat(ArgsT&&... t_args)
    {
        return (to_string(std::forward<ArgsT>(t_args)) + ...);
    }

    /**
    * @brief
    *     Get the EOL control sequence string corresponding
    *     to the given line-ending control sequence.
    */
    constexpr string eol(Eol t_eol) noexcept
    {
        string eol_buffer;

        switch (t_eol)
        {
            case Eol::cr:
                eol_buffer = CR;
                break;
            case Eol::crlf:
                eol_buffer = CRLF;
                break;
            default:
                eol_buffer = LF;
                break;
        }
        return eol_buffer;
    }

    /**
    * @brief
    *     Erase all occurrences of the specified substring from the given data.
    */
    constexpr string& erase(string& t_data, const string& t_sub)
    {
        return replace(t_data, t_sub, "");
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
        const string msg{replace(t_msg, concat("\\", MOD), MOD_PLACEHOLDER)};

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
        return replace(fmt_msg, MOD_PLACEHOLDER, MOD);
    }

    /**
    * @brief
    *     Join the values of the given range using the specified delimiter.
    */
    template<LShiftRange R = vector<string>>
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
    template<LShiftRange R = vector<string>>
    constexpr string join_lines(const R& t_range)
    {
        return join(t_range, LF);
    }

    /**
    * @brief
    *     Replace all line-ending control sequences in the
    *     given data with the specified control sequence.
    */
    constexpr string& normalize_eol(string& t_data, Eol t_eol = Eol::lf)
    {
        return replace(t_data, {CRLF, CR, LF}, eol(t_eol));
    }

    /**
    * @brief
    *     Replace all line-ending control sequences in the
    *     given data with the specified control sequence.
    */
    constexpr string normalize_eol(const string& t_data, Eol t_eol = Eol::lf)
    {
        return replace(t_data, {CRLF, CR, LF}, eol(t_eol));
    }

    /**
    * @brief
    *     Replace all line-ending control sequences in the
    *     given data with the specified control sequence.
    */
    template<LShift T>
    constexpr string normalize_eol(const T& t_data, Eol t_eol = Eol::lf)
    {
        return normalize_eol(to_string(t_data), t_eol);
    }

    /**
    * @brief
    *     Create a whitespace padding buffer of the given size.
    */
    constexpr string pad(size_t t_size)
    {
        return string(t_size, ' ');
    }

    /**
    * @brief
    *     Append whitespace padding to the given data so the
    *     resulting size matches the specified result size.
    */
    constexpr string pad(const string& t_data, size_t t_result_size)
    {
        string padded_data{t_data};
        const size_t delta{t_result_size - t_data.size()};

        if (valid_offset(delta))
        {
            padded_data += pad(delta);
        }
        return padded_data;
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
            if (!is_npos(offset = t_data.find(sub)))
            {
                buffer = t_data.substr(0_st, offset);
                break;
            }
        }
        return is_npos(offset) ? t_data : buffer;
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
            if (!is_npos(offset = t_data.rfind(sub)))
            {
                buffer = t_data.substr(0_st, offset);
                break;
            }
        }
        return is_npos(offset) ? t_data : buffer;
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
    *     Wrap the given data into lines using the specified line size.
    */
    constexpr string wrap(string&& t_data, size_t t_ln_size = LN_SIZE_DEFAULT)
    {
        string result;
        size_t offset{0_st};

        normalize_eol(t_data);
        replace(t_data, "\t", "    ");

        --(t_ln_size = t_ln_size < LN_SIZE_MIN ? LN_SIZE_MIN : t_ln_size);

        while (t_data.size() > t_ln_size)
        {
            size_t eol_index{t_data.find(LF)};
            const string buffer{t_data.substr(0_st, eol_index)};

            // Do not wrap colorized lines
            if (buffer.starts_with(CSI))
            {
                result += buffer + LF;
                t_data.erase(0_st, eol_index + 1_st);
                continue;
            }

            // Calculate EOL and indentation positions
            if (is_npos(eol_index) || eol_index > t_ln_size)
            {
                const size_t prev_offset{offset};
                offset = indent_offset(t_data);

                // Continue wrapping with previous indentation
                if (offset == 0 && valid_offset(prev_offset))
                {
                    offset = prev_offset;
                }
                const size_t padded_ln_size{t_ln_size - offset};

                // Wrap by size if no delimiter found
                if (is_npos(eol_index = t_data.find_last_of(WRAP_CHARS, padded_ln_size)))
                {
                    eol_index = padded_ln_size;
                }
            }
            result += t_data.substr(0_st, eol_index);

            // Include delimiter in results
            if (!contains(to_string(TRIM_CHARS), t_data[eol_index]))
            {
                result += t_data[eol_index];
            }

            result += LF;
            t_data.erase(0_st, eol_index + 1_st);

            if (valid_offset(offset) && indent_offset(t_data) == 0_st)
            {
                result += pad(offset);
            }
            else  // Reset indentation offset
            {
                offset = 0_st;
            }
        }
        result += t_data;

        return std::move(result);
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
    constexpr vector<string> split(const string& t_data,
                                   const string& t_delim,
                                   size_t t_count = string::npos)
    {
        size_t index;
        size_t offset{0_st};

        vector<string> buffer;
        t_count = t_count == 0_st ? string::npos : t_count;

        while (!is_npos(index = t_data.find(t_delim, offset)))
        {
            if (buffer.size() + 1_st == t_count)
            {
                break;
            }
            buffer.push_back(t_data.substr(offset, index - offset));
            offset = index + t_delim.size();
        }
        buffer.push_back(t_data.substr(offset));

        return buffer;
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
        const vector<string> vect{split(t_data, t_delim, N)};

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
    constexpr vector<string> arg_vector(int t_argc, char* t_argv[])
    {
        vector<string> arg_vect;

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
    template<LShiftRange R = vector<string>>
    constexpr vector<string> str_vector(const R& t_range, size_t t_count = string::npos)
    {
        vector<string> vect;
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
    constexpr R& sort(R& t_range, F t_sort_pred = {}, P t_proj_func = {})
    {
        ranges::sort(t_range, t_sort_pred, t_proj_func);
        return t_range;
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
        return sort(buffer, t_sort_pred, t_proj_func);
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

    template<StringRange R = vector<string>>
    bool is_integral(const R& t_range, bool t_unsigned = false);

    bool matches(const string& t_data, const string& t_rgx_pattern);

    uint16_t to_word(const string& t_data);
    uint_t to_uint(const string& t_data);

    string to_lower(const string& t_data);
    string to_upper(const string& t_data);
    string underline(const string& t_data, char t_ln_char = CHAR_DASH);
    string underline(const string& t_data, Color t_color, char t_ln_char = CHAR_DASH);

    template<Range R = vector<string>, class T = range_value_t<R>>
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
