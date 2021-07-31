/*
*  util.h
*  ------
*  Header file for string and console utilities
*/
#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <string>
#include <vector>
#include "../properties/autoprop.h"

namespace scan
{
    /// ***
    /// String and console utility class
    /// ***
    class Util
    {
    private:  /* Types */
        using string       = std::string;
        using stringstream = std::stringstream;
        using vector_s     = std::vector<string>;
        using wstring      = std::wstring;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Destructor */
        virtual ~Util() = default;

    private:  /* Constructors (deleted) */
        Util() = delete;
        Util(const Util &) = delete;

    public:  /* Methods */
        static size_t count(const string &t_str, const char &t_ch);

        static vector_s split(const string &t_data, const string &t_delim);

        static vector_s split(const string &t_data,
                              const string &t_delim,
                              const size_t &t_max_split);

        template<class T>
        static string fstr(const string &t_data, const T &t_arg);

        static string str(const wstring &t_wdata);

        static string strip(const string &t_data,
                            const char &t_ch,
                            const bool &t_space = true);

        static string to_lower(const string &t_data);

        template<class T>
        static vector_s to_vector_s(const vector<T> &t_vect,
                                    const size_t &t_count = 0);

        static wstring wstr(const string &t_data);
    };
}

/// ***
/// Interpolate string with argument at '%' position(s)
/// ***
template<class T>
inline std::string scan::Util::fstr(const string &t_data, const T &t_arg)
{
    // Missing '%' substring
    if (t_data.find('%') == string::npos)
    {
        throw ArgEx("t_data", "Missing format substring: '%'");
    }
    stringstream ss;

    for (size_t i{ 0 }; i < t_data.size(); i++)
    {
        (t_data[i] == '%') ? (ss << t_arg) : (ss << t_data[i]);
    }
    return ss.str();
}

/// ***
/// Convert an integral vector to a string vector
/// ***
template<class T>
inline scan::Util::vector_s scan::Util::to_vector_s(const vector<T> &t_vect,
                                                    const size_t &t_count) {
    // Integral vector assertion
    static_assert(std::is_integral_v<T>, "Expected an integral vector");

    size_t size;

    // Determine maximum vector size
    if ((t_count > 0) && (t_count < t_vect.size()))
    {
        size = t_count;
    }
    else
    {
        size = t_vect.size();
    }
    vector_s svect;

    for (size_t i{ 0 }; i < size; i++)
    {
        svect.push_back(std::to_string(static_cast<size_t>(t_vect[i])));
    }
    return svect;
}

#endif // !UTIL_H
