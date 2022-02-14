/*
*  util.h
*  ------
*  Header file for string and data-type utilities
*/
#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <string>
#include <vector>

namespace scan
{
    /// ***
    /// String and data-type utility class
    /// ***
    class Util final
    {
    private:  /* Type Aliases */
        using sstream  = std::stringstream;
        using string   = std::string;
        using wstring  = std::wstring;
        using vector_s = std::vector<string>;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Destructor */
        virtual ~Util() = default;

    private:  /* Constructors (deleted) */
        Util() = delete;
        Util(const Util &) = delete;

    public:  /* Methods */
        static bool ends_with(const string &t_data, const string &t_sub_str);
        static bool ends_with(const string &t_data, const vector_s &t_sub_strs);
        static bool is_integral(const string &t_data);
        static bool starts_with(const string &t_data, const string &t_sub_str);

        static size_t count(const string &t_data, const char &t_ch);

        template<class T, class ...Args>
        static string fstr(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        static string lstrip(const string &t_data);

        static string replace(const string &t_data,
                              const string &t_old_sub,
                              const string &t_new_sub);

        static string replace(const string &t_data,
                              const vector_s &t_old_subs,
                              const string &t_new_sub);

        static string rstrip(const string &t_data);
        static string str(const wstring &t_wdata);
        static string strip(const string &t_data);
        static string to_lower(const string &t_data);
        static string to_upper(const string &t_data);

        static wstring wstr(const string &t_data);

        static vector_s split(const string &t_data, const string &t_delim);

        static vector_s split(const string &t_data,
                              const string &t_delim,
                              const size_t &t_max_split);

        template<class T>
        static vector_s to_vector_s(const vector<T> &t_vect,
                                    const size_t &t_count = 0);

    private:  /* Methods */
        static string fstr(const string &t_data);
    };
}

/// ***
/// Interpolate one or more arguments in the given string at '%' positions
/// ***
template<class T, class ...Args>
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
/// Convert an integral vector to a string vector
/// ***
template<class T>
inline scan::Util::vector_s scan::Util::to_vector_s(const vector<T> &t_vect,
                                                    const size_t &t_count) {

    static_assert(std::is_integral_v<T>, "Expected an integral vector");

    const bool is_count_specified{ (t_count > 0) && (t_count < t_vect.size()) };
    const size_t max_count{ is_count_specified ? t_count : t_vect.size() };

    vector_s svect;

    // Add elements to vector
    for (size_t i{ 0 }; i < max_count; i++)
    {
        svect.push_back(std::to_string(t_vect[i]));
    }
    return svect;
}

#endif // !UTIL_H
