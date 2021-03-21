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

namespace Scan
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
        static constexpr char CR[] = "\r";          // Carriage-return
        static constexpr char LF[] = "\n";          // Unix EOL (line feed)
        static constexpr char CRLF[] = {*CR, *LF};  // NT EOL (CR-LF)

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
        static void error(const string &msg);
        static void errorf(const string &msg, const string &arg);
        static void except(const ArgEx &ex);
        static void print(const string &msg);
        static void printf(const string &msg, const string &arg);
        static void warn(const string &msg);

        static const int enable_vt();

        static const size_t count(const string &str, const char &ch);
        static const char *itoc(const llong &num);

        static const vector_s split(const string &data, const string &delim);

        static const vector_s split(const string &data,
                                    const string &delim,
                                    const size_t &max_split);
        template<class T>
        static const string fmt(const string &msg, const T &arg);

        static const string indent(const string &data,
                                   const uint &tab_size = 4,
                                   const bool skip_first = false);

        static const string itos(const llong &num);

        static const string strip(const string &data, const char &match_ch,
                                                      const bool &space = true);

        static const string to_lower(const string &data);
        static const string utf8(const wstring &data_w);

        static const wstring utf16(const string &data);

    private:  /* Methods */
        static void print(const FgColor &fg, const string &msg);
    };
}

/// ***
/// Interpolate string with argument at '%' position(s)
/// ***
template<class T>
inline const std::string Scan::Util::fmt(const string &msg, const T &arg)
{
    if (msg.find('%') == -1)
    {
        throw ArgEx("msg", "Missing format char: '%'");
    }
    std::stringstream ss;

    // Populate stringstream data
    for (int i = {0}; i < msg.length(); i++)
    {
        (msg[i] == '%') ? (ss << arg) : (ss << msg[i]);
    }
    return ss.str();
}

#endif // !UTIL_H
