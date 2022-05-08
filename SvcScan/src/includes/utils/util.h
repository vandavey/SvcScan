/*
*  util.h
*  ------
*  Header file for algorithms and data-type utilities
*/
#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <string>
#include <vector>
#include <boost/range/algorithm/count.hpp>
#include "../constraints/typeconcepts.h"

namespace
{
    namespace ranges = std::ranges;
}

namespace scan
{
    /// ***
    /// String and data-type utility class
    /// ***
    class Util final
    {
    private:  /* Type Aliases */
        using sstream      = std::stringstream;
        using str_iterator = std::string::const_iterator;
        using string       = std::string;
        using wstring      = std::wstring;

        template<std::forward_iterator T>
        using iter_range = boost::iterator_range<T>;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Constructors & Destructor */
        Util() = delete;
        Util(const Util &) = delete;

        virtual ~Util() = default;

    public:  /* Methods */
        static bool ends_with(const string &t_data, const string &t_sub_str);

        static bool ends_with(const string &t_data,
                              const vector<string> &t_sub_vect);

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

        static size_t count(const string &t_data, const char &t_ch);
        static size_t count(const string &t_data, const string &t_sub);

        template<Range T, std::forward_iterator I>
        static size_t distance(const T &t_range, const I &t_iter);

        template<std::forward_iterator T>
        static size_t distance(const T &t_beg_iter, const T &t_end_iter);

        template<LShift T, LShift ...Args>
        static string fstr(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        static string lstrip(const string &t_data);

        static string replace(const string &t_data,
                              const string &t_old_sub,
                              const string &t_new_sub);

        static string replace(const string &t_data,
                              const vector<string> &t_old_subs,
                              const string &t_new_sub);

        static string rstrip(const string &t_data);
        static string str(const wstring &t_wdata);
        static string strip(const string &t_data);

        static string substr(const string &t_data,
                             const str_iterator &t_beg_it,
                             const str_iterator &t_end_it);

        static string to_lower(const string &t_data);
        static string to_upper(const string &t_data);

        static wstring wstr(const string &t_data);

        static vector<string> split(const string &t_data, const string &t_delim);

        static vector<string> split(const string &t_data,
                                    const string &t_delim,
                                    const size_t &t_max_split);

        template<LShift T>
        static vector<string> to_str_vector(const vector<T> &t_vect,
                                            const size_t &t_count = 0);

    private:  /* Methods */
        static string fstr(const string &t_data);
    };
}

/// ***
/// Calculate the distance from the beginning of the range to the range iterator
/// ***
template<scan::Range T, std::forward_iterator I>
inline size_t scan::Util::distance(const T &t_range, const I &t_it)
{
    return distance(t_range.begin(), t_it);
}

/// ***
/// Calculate the distance (offset) between the given range iterators
/// ***
template<std::forward_iterator T>
inline size_t scan::Util::distance(const T &t_beg_it, const T &t_end_it)
{
    size_t offset{ 0 };

    if (t_beg_it != t_end_it)
    {
        offset = static_cast<size_t>(ranges::distance(t_beg_it, t_end_it));
    }
    return offset;
}

/// ***
/// Interpolate one or more arguments in the given string at '%' positions
/// ***
template<scan::LShift T, scan::LShift ...Args>
inline std::string scan::Util::fstr(const string &t_msg,
                                    const T &t_arg,
                                    const Args &...t_args) {
    sstream ss;

    // Interpolate arguments variadically
    for (const char *p{ &t_msg[0] }; *p != '\0'; p++)
    {
        if (*p == '%')
        {
            ss << t_arg << fstr(++p, t_args...);
            break;
        }
        ss << *p;
    }
    return ss.str();
}

/// ***
/// Convert an integral vector to a vector of strings
/// ***
template<scan::LShift T>
inline std::vector<std::string> scan::Util::to_str_vector(const vector<T> &t_vect,
                                                          const size_t &t_count) {

    const bool is_count_specified{ (t_count > 0) && (t_count < t_vect.size()) };
    const size_t max_count{ is_count_specified ? t_count : t_vect.size() };

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
