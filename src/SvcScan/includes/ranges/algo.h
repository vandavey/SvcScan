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
#include <array>
#include <bit>
#include <cstdint>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>
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
    *     Calculate the FNV-1A bitwise hash of the the given value.
    */
    consteval size_t fnv1a_hash(Hashable auto t_value) noexcept
    {
        const byte_array_t bytes{std::bit_cast<byte_array_t<sizeof t_value>>(t_value)};
        const uint8_t* byte_ptr{&bytes[0]};

        size_t hash{FNV_OFFSET_BASIS};

        for (size_t i{0_sz}; i < sizeof t_value; ++i)
        {
            hash ^= static_cast<size_t>(byte_ptr[i]);
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
        /// @brief  Modulus string placeholder wrapper.
        constexpr CString<~fnv1a_hash(*MOD)> MOD_PLACEHOLDER = {};
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
    *     Get the maximum value from the given numbers.
    */
    template<Numeric... ArgsT>
        requires AtLeastOne<ArgsT...>
    constexpr Numeric auto (max)(ArgsT&&... t_nums)
    {
        return (ranges::max)({std::forward<ArgsT>(t_nums)...});
    }

    /**
    * @brief
    *     Get the minimum value from the given numbers.
    */
    template<Numeric... ArgsT>
        requires AtLeastOne<ArgsT...>
    constexpr Numeric auto (min)(ArgsT&&... t_nums)
    {
        return (ranges::min)({std::forward<ArgsT>(t_nums)...});
    }

    /**
    * @brief
    *     Get an iterator to the beginning of the given range.
    */
    template<Range R>
    constexpr Iter auto begin(R&& t_range) noexcept
    {
        return Const<R> ? ranges::cbegin(t_range) : ranges::begin(t_range);
    }

    /**
    * @brief
    *     Get an iterator to the end of the given range.
    */
    template<Range R>
    constexpr Iter auto end(R&& t_range) noexcept
    {
        return Const<R> ? ranges::cend(t_range) : ranges::end(t_range);
    }

    /**
    * @brief
    *     Find the first matching value in the given range.
    */
    template<Range R, SameByValue<range_value_t<R>> T>
    constexpr Iter auto find(const R& t_range, T&& t_value)
    {
        return ranges::find(t_range, std::forward<T>(t_value));
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
            size_t index{0_sz};

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
    constexpr string& replace(string& t_data,
                              const StringLikeRange auto& t_old_subs,
                              const string& t_new_sub)
    {
        if (!t_data.empty())
        {
            for (const StringLike auto& old_sub : t_old_subs)
            {
                replace(t_data, static_cast<string>(old_sub), t_new_sub);
            }
        }
        return t_data;
    }

    /**
    * @brief
    *     Replace all substring occurrences in the given data with a new substring.
    */
    constexpr string replace(const string& t_data,
                             const StringLikeRange auto& t_old_subs,
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
    constexpr string to_string(T&& t_value) noexcept(StringLike<T>)
    {
        string result;

        if constexpr (StringLike<T>)
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
    *     Copy all values from the given range to the other specified range.
    */
    template<BasicRange R, PushableRangeOf<range_value_t<R>> OutR>
    constexpr OutR& copy(R&& t_from_range, OutR& t_to_range)
    {
        ranges::copy(std::forward<R>(t_from_range), std::back_inserter(t_to_range));
        return t_to_range;
    }

    /**
    * @brief
    *     Move all values from the given range to the other specified range.
    */
    template<BasicRange R, PushableRangeOf<range_value_t<R>> OutR>
    constexpr OutR& move(R&& t_from_range, OutR& t_to_range)
    {
        ranges::move(std::forward<R>(t_from_range), std::back_inserter(t_to_range));
        return t_to_range;
    }

    /**
    * @brief
    *     Transfer all values from the given range to the other specified range.
    */
    template<BasicRange R, PushableRangeOf<range_value_t<R>> OutR>
    constexpr OutR& transfer(R&& t_from_range, OutR& t_to_range)
    {
        return Const<R> ? copy(std::forward<R>(t_from_range), t_to_range)
                        : move(std::forward<R>(t_from_range), t_to_range);
    }

    /**
    * @brief
    *     Determine whether a given value is equal to any
    *     of the other specified values which follow it.
    */
    template<class T, EqComparable<T>... ArgsT>
        requires AtLeastOne<ArgsT...>
    constexpr bool any_equal(const T& t_arg, ArgsT&&... t_args) noexcept
    {
        bool equal;

        if constexpr (StringLike<T>)
        {
            equal = ((to_string(t_arg) == std::forward<ArgsT>(t_args)) || ...);
        }
        else
        {
            equal = ((t_arg == std::forward<ArgsT>(t_args)) || ...);
        }
        return equal;
    }

    /**
    * @brief
    *     Determine whether the given range contains the specified value.
    */
    template<Range R, SameByValue<range_value_t<R>> T>
    constexpr bool contains(const R& t_range, T&& t_value)
    {
        return find(t_range, std::forward<T>(t_value)) != end(t_range);
    }

    /**
    * @brief
    *     Determine whether an unsigned integral sum causes an integer overflow.
    */
    constexpr bool sum_overflow(Unsigned auto t_lhs_num, Unsigned auto t_rhs_num) noexcept
    {
        return static_cast<decltype(t_lhs_num)>(t_lhs_num + t_rhs_num) < t_lhs_num;
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
    template<Range R>
    constexpr size_t count(const R& t_range, const range_value_t<R>& t_value)
    {
        return static_cast<size_t>(ranges::count(t_range, t_value));
    }

    /**
    * @brief
    *     Count the number of substring occurrences in the given data.
    */
    constexpr size_t count(const string& t_data, const string& t_sub) noexcept
    {
        size_t count{0_sz};
        size_t offset{0_sz};

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
        size_t count{0_sz};
        size_t offset{0_sz};

        size_t index;
        size_t match_offset{NPOS};

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
    constexpr size_t max_key_size(const StringMap auto& t_map)
    {
        size_t max_size{0_sz};

        for (const StringPair auto& pair : t_map)
        {
            max_size = (max)(max_size, pair.first.size());
        }
        return max_size;
    }

    /**
    * @brief
    *     Get the current maximum key size from the given maps.
    */
    constexpr size_t max_key_size(const StringMap auto&... t_maps)
        requires AtLeastOne<decltype(t_maps)...>
    {
        return (max)(max_key_size(t_maps)...);
    }

    /**
    * @brief
    *     Get the size of the given range.
    */
    template<Range R>
    constexpr range_size_t<R> size(R&& t_range) noexcept
    {
        return ranges::size(t_range);
    }

    /**
    * @brief
    *     Get the absolute value of the given number.
    */
    constexpr Numeric auto abs(Numeric auto t_num) noexcept
    {
        if constexpr (Signed<decltype(t_num)>)
        {
            t_num = t_num >= 0 ? t_num : -t_num;
        }
        return t_num;
    }

    /**
    * @brief
    *     Convert the given arguments to strings and concatenate the results.
    */
    template<LShift... ArgsT>
        requires AtLeastTwo<ArgsT...>
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
        return replace(t_data, t_sub, {});
    }

    /**
    * @brief
    *     Erase all occurrences of the specified substring from the given data.
    */
    constexpr string erase(const string& t_data, const string& t_sub)
    {
        return replace(t_data, t_sub, {});
    }

    /**
    * @brief
    *     Interpolate one or more arguments in the given string at
    *     the modulus (`%`) positions. Modulus literals can be
    *     included by prefixing them with back-slashes (`\\%`).
    */
    template<LShift T, LShift... ArgsT>
    constexpr string fstr(const string& t_msg, T&& t_arg, ArgsT&&... t_args)
    {
        // Replace escaped moduli with placeholders
        const string msg{replace(t_msg, concat("\\", MOD), MOD_PLACEHOLDER)};

        string fmt_msg;

        for (const char* p{&msg[0]}; *p != CHAR_NULL; p++)
        {
            if (*p == *MOD)
            {
                fmt_msg += to_string(std::forward<T>(t_arg));

                if constexpr (AtLeastOne<ArgsT...>)
                {
                    fmt_msg += fstr(++p, to_string(std::forward<ArgsT>(t_args))...);
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
    constexpr string join(const LShiftRange auto& t_range, const string& t_delim)
    {
        string result;

        for (Iter auto it{begin(t_range)}; it != end(t_range); ++it)
        {
            result += to_string(*it);

            if (it + 1 != end(t_range))
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
    constexpr string join_lines(const LShiftRange auto& t_range)
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
        return replace(t_data, vector<string>{CRLF, CR, LF}, eol(t_eol));
    }

    /**
    * @brief
    *     Replace all line-ending control sequences in the
    *     given data with the specified control sequence.
    */
    constexpr string normalize_eol(const string& t_data, Eol t_eol = Eol::lf)
    {
        return replace(t_data, vector<string>{CRLF, CR, LF}, eol(t_eol));
    }

    /**
    * @brief
    *     Replace all line-ending control sequences in the
    *     given data with the specified control sequence.
    */
    constexpr string normalize_eol(const LShift auto& t_data, Eol t_eol = Eol::lf)
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
        data.erase(0_sz, data.find_first_not_of(TRIM_CHARS));
        return data;
    }

    /**
    * @brief
    *     Remove all trailing whitespace characters from the given data.
    */
    constexpr string trim_right(const string& t_data)
    {
        string data{t_data};
        data.erase(data.find_last_not_of(TRIM_CHARS) + 1_sz);
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
    constexpr string up_to_first(const string& t_data, const StringLike auto&... t_args)
        requires AtLeastOne<decltype(t_args)...>
    {
        string buffer;
        size_t offset{NPOS};

        for (const StringLike auto& sub : {t_args...})
        {
            if (!is_npos(offset = t_data.find(sub)))
            {
                buffer = t_data.substr(0_sz, offset);
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
    constexpr string up_to_last(const string& t_data, const StringLike auto&... t_args)
        requires AtLeastOne<decltype(t_args)...>
    {
        string buffer;
        size_t offset{NPOS};

        for (const StringLike auto& sub : {t_args...})
        {
            if (!is_npos(offset = t_data.rfind(sub)))
            {
                buffer = t_data.substr(0_sz, offset);
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
        size_t offset{0_sz};

        normalize_eol(t_data);
        replace(t_data, "\t", "    ");

        --(t_ln_size = t_ln_size < LN_SIZE_MIN ? LN_SIZE_MIN : t_ln_size);

        while (t_data.size() > t_ln_size)
        {
            size_t eol_index{t_data.find(LF)};
            const string buffer{t_data.substr(0_sz, eol_index)};

            // Do not wrap colorized lines
            if (buffer.starts_with(CSI))
            {
                result += buffer + LF;
                t_data.erase(0_sz, eol_index + 1_sz);
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
            result += t_data.substr(0_sz, eol_index);

            // Include delimiter in results
            if (!contains(to_string(TRIM_CHARS), t_data[eol_index]))
            {
                result += t_data[eol_index];
            }

            result += LF;
            t_data.erase(0_sz, eol_index + 1_sz);

            if (valid_offset(offset) && indent_offset(t_data) == 0)
            {
                result += pad(offset);
            }
            else  // Reset indentation offset
            {
                offset = 0_sz;
            }
        }

        return result + t_data;
    }

    /**
    * @brief
    *     Create a range view inclusively containing all integers within
    *     the given range bounds. The maximum and minimum values will be
    *     swapped when the maximum value is less than the minimum value.
    */
    template<Integral T>
    constexpr ViewOf<T> auto iota(T t_min, T t_max)
    {
        return views::iota((min)(t_min, t_max), (max)(t_min, t_max) + 1);
    }

    /**
    * @brief
    *     Split the given data using the specified delimiter into a vector
    *     whose size is less than or equal to the specified value count.
    */
    constexpr vector<string> split(const string& t_data,
                                   const string& t_delim,
                                   size_t t_count = NPOS)
    {
        size_t index;
        size_t offset{0_sz};

        vector<string> buffer;
        t_count = t_count == 0_sz ? NPOS : t_count;

        while (!is_npos(index = t_data.find(t_delim, offset)))
        {
            if (buffer.size() + 1_sz == t_count)
            {
                break;
            }
            buffer.emplace_back(t_data.substr(offset, index - offset));
            offset = index + t_delim.size();
        }
        buffer.emplace_back(t_data.substr(offset));

        return buffer;
    }

    /**
    * @brief
    *     Split the given data into a fixed-size array using the specified delimiter.
    */
    template<size_t N>
    constexpr string_array_t<N> split(const string& t_data, const string& t_delim)
        requires(!is_npos(N) && N > 0)
    {
        string_array_t<N> buffer;
        const vector<string> vect{split(t_data, t_delim, N)};

        for (size_t i{0_sz}; i < vect.size(); i++)
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
                    arg_vect.emplace_back(t_argv[i]);
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
    constexpr vector<string> str_vector(const LShiftRange auto& t_range,
                                        size_t t_count = NPOS)
    {
        vector<string> vect;
        t_count = t_count == 0_sz ? NPOS : t_count;

        for (size_t i{0_sz}; i < size(t_range) && i < t_count; i++)
        {
            vect.emplace_back(to_string(t_range[i]));
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
    bool is_integral(const StringLikeRange auto& t_range, bool t_unsigned = false);
    bool matches(const string& t_data, const string& t_rgx_pattern);

    uint16_t to_word(const string& t_data);
    uint_t to_uint(const string& t_data);

    string to_lower(const string& t_data);
    string to_upper(const string& t_data);
    string underline(const string& t_data, char t_ln_char = CHAR_DASH);
    string underline(const string& t_data, Color t_color, char t_ln_char = CHAR_DASH);

    vector<IndexedArg> enumerate(const Range auto& t_range, const string& t_filter = {});
}

/**
* @brief
*     Determine whether all the given strings contain only integral numbers.
*     Optionally consider only unsigned integral numbers as valid.
*/
inline bool scan::algo::is_integral(const StringLikeRange auto& t_range, bool t_unsigned)
{
    return ranges::all_of(t_range, [&t_unsigned](const StringLike auto& l_str) -> bool
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
inline std::vector<scan::IndexedArg> scan::algo::enumerate(const Range auto& t_range,
                                                           const string& t_filter)
{
    vector<IndexedArg> indexed_args;

    // Enumerate all range values
    for (size_t i{0_sz}; i < size(t_range); i++)
    {
        if (t_filter.empty() || matches(t_range[i], t_filter))
        {
            indexed_args.emplace_back(IndexedArg{i, t_range[i]});
        }
    }
    return indexed_args;
}

#endif // !SCAN_ALGO_H
