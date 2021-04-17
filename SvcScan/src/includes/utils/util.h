/*
*  util.h
*  ------
*  Header file for string and standard stream manipulation
*/
#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "../except/nullargex.h"
#include "../properties/autoprop.h"

namespace scan
{
    /// ***
    /// String and standard stream manipulation utilities
    /// ***
    class Util
    {
    private:  /* Types */
        enum class FgColor : short;

        using llong = long long;
        using uint = unsigned int;
        using ulong = unsigned long;

        using string = std::string;
        using wstring = std::wstring;

        using map_cs = std::map<FgColor, string>;
        using vector_s = std::vector<string>;

    public:  /* Constants */
        static constexpr char CR[] = "\r";               // Carriage-return
        static constexpr char LF[] = "\n";               // Unix EOL (LF)
        static constexpr char CRLF[]{ *CR, *LF, '\0' };  // NT EOL (CR-LF)

    private:  /* Constants */
        static constexpr char RESET[] = "\033[0m";  // Ansi reset sequence

        // Ansi color escape sequences
        static constexpr char CYAN[] = "\033[38;2;0;255;255m";
        static constexpr char RED[] = "\033[38;2;246;0;0m";
        static constexpr char YELLOW[] = "\033[38;2;250;230;39m";

    public:  /* Fields */
        static AutoProp<bool> vt_enabled;  // VT escape processing
        
    private:  /* Fields */
        static map_cs m_icons;  // Color-icon dictionary

    public:  /* Destructor */
        virtual ~Util() = default;

    private:  /* Constructors (deleted) */
        Util() = delete;
        Util(const Util &) = delete;

    public:  /* Methods */
        static void error(const string &t_msg);
        static void errorf(const string &t_msg, const string &t_arg);
        static void except(const ArgEx &t_ex);
        static void print(const string &t_msg);
        static void printf(const string &t_msg, const string &t_arg);
        static void warn(const string &t_msg);

        static const int enable_vt();

        static const size_t count(const string &t_str, const char &t_ch);
        static const char *itoc(const llong &t_num);

        static const vector_s split(const string &t_data,
                                    const string &t_delim);

        static const vector_s split(const string &t_data,
                                    const string &t_delim,
                                    const size_t &t_max_split);
        template<class T>
        static const string fmt(const string &t_msg, const T &t_arg);

        static const string indent(const uint &t_size,
                                   const string &t_data,
                                   const bool t_skip_first = false);

        static const string itos(const llong &t_num);

        static const string strip(const string &t_data,
                                  const char &t_ch,
                                  const bool &t_space = true);

        static const string to_lower(const string &t_data);
        static const string utf8(const wstring &t_wdata);

        static const wstring utf16(const string &t_data);

    private:  /* Methods */
        static void print(const FgColor &t_fg, const string &t_msg);
    };
}

/// ***
/// Interpolate string with argument at '%' position(s)
/// ***
template<class T>
inline const std::string scan::Util::fmt(const string &t_msg, const T &t_arg)
{
    if (t_msg.find('%') == -1)
    {
        throw ArgEx("t_msg", "Missing format char: '%'");
    }
    std::stringstream ss;

    // Populate stringstream data
    for (int i{ 0 }; i < t_msg.length(); i++)
    {
        (t_msg[i] == '%') ? (ss << t_arg) : (ss << t_msg[i]);
    }
    return ss.str();
}

#endif // !UTIL_H
