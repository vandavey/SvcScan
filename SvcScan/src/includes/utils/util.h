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
#include "../except/logicex.h"
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
        enum class FgColor : short
        {
            cyan,   // Cyan foreground
            red,    // Red foreground
            yellow  // Yellow foreground
        };

        using llong = long long;
        using uint  = unsigned int;
        using ulong = unsigned long;

        using string  = std::string;
        using wstring = std::wstring;

        using map_cs   = std::map<FgColor, string>;
        using vector_s = std::vector<string>;

    public:  /* Constants */
        static constexpr char CR[] = "\r";               // Carriage-return
        static constexpr char LF[] = "\n";               // Unix EOL (LF)
        static constexpr char CRLF[]{ *CR, *LF, '\0' };  // NT EOL (CR-LF)

    private:  /* Constants */
        static constexpr char RESET[] = "\033[0m";  // Ansi reset sequence

        // Ansi color escape sequences
        static constexpr char CYAN[]   = "\033[38;2;0;255;255m";
        static constexpr char RED[]    = "\033[38;2;246;0;0m";
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

        template<class T>
        static void errorf(const string &t_msg, const T &t_arg);

        static void except(const ArgEx &t_ex);
        static void except(const LogicEx &t_ex);
        static void print(const string &t_msg);

        template<class T>
        static void printf(const string &t_msg, const T &t_arg);

        static void warn(const string &t_msg);

        static bool ends_with(const string &t_spath, const string &t_sub);
        static bool ends_with(const string &t_spath, const vector_s &t_svect);

        static int enable_vt();
        static size_t count(const string &t_str, const char &t_ch);

        static vector_s split(const string &t_data, const string &t_delim);

        static vector_s split(const string &t_data,
                              const string &t_delim,
                              const size_t &t_max_split);
        template<class T>
        static string fstr(const string &t_data, const T &t_arg);

        static string strip(const string &t_data,
                            const char &t_ch,
                            const bool &t_space = true);

        static string to_lower(const string &t_data);
        static string str(const wstring &t_wdata);

        static wstring wstr(const string &t_data);

    private:  /* Methods */
        static void print(const FgColor &t_fg, const string &t_msg);
    };
}

/// ***
/// Write formatted error message to standard error
/// ***
template<class T>
inline void scan::Util::errorf(const string &t_msg, const T &t_arg)
{
    if (t_msg.find('%') == -1)
    {
        throw ArgEx("t_msg", "Missing format character");
    }
    print(FgColor::red, fstr(t_msg, t_arg));
}

/// ***
/// Write general information to standard output
/// ***
template<class T>
inline void scan::Util::printf(const string &t_msg, const T &t_arg)
{
    if (t_msg.find('%') == -1)
    {
        throw ArgEx("t_msg", "Missing format character");
    }
    print(FgColor::cyan, fstr(t_msg, t_arg));
}

/// ***
/// Interpolate string with argument at '%' position(s)
/// ***
template<class T>
inline std::string scan::Util::fstr(const string &t_data, const T &t_arg)
{
    if (t_data.find('%') == -1)
    {
        throw ArgEx("t_data", "Missing format char: '%'");
    }
    std::stringstream ss;

    // Populate stringstream data
    for (int i{ 0 }; i < t_data.length(); i++)
    {
        (t_data[i] == '%') ? (ss << t_arg) : (ss << t_data[i]);
    }
    return ss.str();
}

#endif // !UTIL_H
