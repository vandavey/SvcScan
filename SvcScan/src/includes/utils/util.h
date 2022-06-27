/*
*  util.h
*  ------
*  Header file for range and string utilities
*/
#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <string>
#include <vector>
#include <boost/range/algorithm/count.hpp>
#include "../concepts/type_concepts.h"

namespace
{
    namespace ranges = std::ranges;
}

namespace scan
{
    /**
    * @brief  Range and string utilities..
    */
    class Util final
    {
    private:  /* Type Aliases */
        using sstream      = std::stringstream;
        using str_iterator = std::string::const_iterator;
        using string       = std::string;
        using wstring      = std::wstring;

        template<RangeIterator T>
        using iter_range = boost::iterator_range<T>;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Constructors & Destructor */
        Util() = delete;
        Util(const Util &) = delete;

        virtual ~Util() = default;

    public:  /* Methods */
        template<ClearableRange R>
        static void clear(R &t_range);

        static bool ends_with(const string &t_data, const string &t_sub_str);

        static bool ends_with(const string &t_data,
                              const vector<string> &t_sub_vect);

        template<Range R>
        static bool empty(const R &t_range);

        static bool is_integral(const string &t_data);
        static bool starts_with(const string &t_data, const string &t_sub_str);

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

        template<LShift T, LShift ...Args>
        static string fstr(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        static string remove(const string &t_data, const string &t_sub);

        static string replace(const string &t_data,
                              const string &t_old_sub,
                              const string &t_new_sub);

        static string replace(const string &t_data,
                              const vector<string> &t_old_subs,
                              const string &t_new_sub);

        static string str(const wstring &t_wdata);

        static string substr(const string &t_data,
                             const str_iterator &t_beg_it,
                             const str_iterator &t_end_it);

        static string to_lower(const string &t_data);
        static string to_upper(const string &t_data);
        static string trim(const string &t_data);
        static string trim_left(const string &t_data);
        static string trim_right(const string &t_data);

        static wstring wstr(const string &t_data);

        static vector<string> split(const string &t_data, const string &t_delim);

        static vector<string> split(const string &t_data,
                                    const string &t_delim,
                                    const size_t &t_max_split);

        template<LShift T>
        static vector<string> to_str_vector(const vector<T> &t_vect,
                                            const size_t &t_count = 0);
    };
}

/**
* @brief  Clear the contents of the given range and release its unused memory.
*/
template<scan::ClearableRange R>
inline void scan::Util::clear(R &t_range)
{
    t_range.clear();
    t_range.shrink_to_fit();
}

/**
* @brief  Determine whether the given range is empty.
*/
template<scan::Range R>
inline bool scan::Util::empty(const R &t_range)
{
    return ranges::empty(t_range);
}

/**
* @brief  Count the number of matching value_type occurrences in the given range.
*/
template<scan::Range R, class T>
inline size_t scan::Util::count(const R &t_range,
                                const T &t_value) requires RangeValue<R, T> {

    return static_cast<size_t>(ranges::count(t_range, t_value));
}

/**
* @brief  Calculate the distance between the beginning of the given
*         range and the specified range iterator.
*/
template<scan::Range R, scan::RangeIterator T>
inline size_t scan::Util::distance(const R &t_range, const T &t_it)
{
    return distance(t_range.begin(), t_it);
}

/**
* @brief  Calculate the distance between the given range iterators.
*/
template<scan::RangeIterator T>
inline size_t scan::Util::distance(const T &t_beg_it, const T &t_end_it)
{
    size_t offset{ 0 };

    if (t_beg_it != t_end_it)
    {
        offset = static_cast<size_t>(ranges::distance(t_beg_it, t_end_it));
    }
    return offset;
}

/**
* @brief  Interpolate one or more arguments in the given string at the
*         modulus (e.g., %) positions. Modulus literals can be included by
*         prefixing them with back-slashes (e.g., \%).
*/
template<scan::LShift T, scan::LShift ...Args>
inline std::string scan::Util::fstr(const string &t_msg,
                                    const T &t_arg,
                                    const Args &...t_args) {
    sstream ss;

    // Replace escaped modulus with placeholders
    const string msg{ Util::replace(t_msg, "\\%", "__MOD__") };

    for (const char *p{ &msg[0] }; *p != '\0'; p++)
    {
        if (*p == '%')
        {
            ss << t_arg;

            // Call method recursively
            if constexpr (sizeof...(t_args) > 0)
            {
                ss << fstr(++p, t_args...);
                break;
            }
            ss << ++p;
            break;
        }
        ss << *p;
    }
    return Util::replace(ss.str(), "__MOD__", "%");
}

/**
* @brief  Convert the given vector to a vector of strings.
*/
template<scan::LShift T>
inline std::vector<std::string> scan::Util::to_str_vector(const vector<T> &t_vect,
                                                          const size_t &t_count) {

    const bool count_specified{ t_count > 0 && t_count < t_vect.size() };
    const size_t max_count{ count_specified ? t_count : t_vect.size() };

    vector<string> svect;

    // Add elements to vector
    for (size_t i{ 0 }; i < max_count; i++)
    {
        sstream ss;
        ss << t_vect[i];
        svect.push_back(ss.str());
    }
    return svect;
}

#endif // !UTIL_H
