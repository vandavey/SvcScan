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
        using string  = std::string;
        using wstring = std::wstring;

        using vector_s = std::vector<string>;

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

        template<class T, class U>
        static string fstr(const string &t_data, const T &t_targ, const U &t_uarg);

        static string str(const wstring &t_wdata);

        static string strip(const string &t_data,
                            const char &t_ch,
                            const bool &t_space = true);

        static string to_lower(const string &t_data);

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
    std::stringstream ss;

    for (size_t i{ 0 }; i < t_data.size(); i++)
    {
        (t_data[i] == '%') ? (ss << t_arg) : (ss << t_data[i]);
    }
    return ss.str();
}

/// ***
/// Interpolate string with arguments at '%' positions
/// ***
template<class T, class U>
inline std::string scan::Util::fstr(const string &t_data,
                                    const T &t_targ,
                                    const U &t_uarg) {
    // Missing format substring
    if (Util::count(t_data, '%') < 2)
    {
        throw ArgEx("t_data", "Missing format substring(s): '%'");
    }
    std::stringstream ss;

    for (size_t i{ 0 }; i < t_data.size(); i++)
    {
        ((i == 0) || (i % 2) == 0) ? (ss << t_targ) : (ss << t_uarg);
    }
    return ss.str();
}

#endif // !UTIL_H
