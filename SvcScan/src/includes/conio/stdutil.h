/*
*  stdutil.h
*  ---------
*  Header file for standard stream utilities
*/
#pragma once

#ifndef STD_UTIL_H
#define STD_UTIL_H

#include <map>
#include <string>
#include "../except/argex.h"
#include "../except/logicex.h"
#include "../properties/autoprop.h"
#include "../utils/util.h"

namespace scan
{
    /// ***
    /// Standard stream manipulation utilities
    /// ***
    class StdUtil final
    {
    private:  /* Types */
        enum class FgColor : short
        {
            cyan,   // Cyan foreground
            green,  // Green foreground
            red,    // Red foreground
            yellow  // Yellow foreground
        };

        using ulong = unsigned long;

        using string = std::string;
        using map_cs = std::map<FgColor, string>;

    public:  /* Constants */
        static constexpr char CR[]   = "\r";    // Carriage-return
        static constexpr char CRLF[] = "\r\n";  // NT EOL (CR-LF)
        static constexpr char LF[]   = "\n";    // Unix EOL (LF)

    private:  /* Constants */
        static constexpr char RESET[] = "\033[0m";  // Ansi reset sequence

        // Ansi color escape sequences
        static constexpr char CYAN[]   = "\033[38;2;0;255;255m";
        static constexpr char GREEN[]  = "\033[38;2;166;226;46m";
        static constexpr char RED[]    = "\033[38;2;246;0;0m";
        static constexpr char YELLOW[] = "\033[38;2;250;230;39m";

    public:  /* Fields */
        static AutoProp<bool> vt_enabled;  // VT escape processing

    private:  /* Fields */
        static map_cs m_icons;  // Color-icon dictionary

    public:  /* Destructor */
        virtual ~StdUtil() = default;

    private:  /* Constructors (deleted) */
        StdUtil() = delete;
        StdUtil(const StdUtil &) = delete;

    public:  /* Methods */
        static void error(const string &t_msg);

        template<class T>
        static void errorf(const string &t_msg, const T &t_arg);

        static void except(const ArgEx &t_ex);
        static void except(const LogicEx &t_ex);
        static void info(const string &t_msg);
        static void print(const string &t_msg);

        template<class T>
        static void printf(const string &t_msg, const T &t_arg);

        static void warn(const string &t_msg);

        static int enable_vt();

    private:  /* Methods */
        static void print(const FgColor &t_fg, const string &t_msg);
    };
}

/// ***
/// Write formatted error message to standard error
/// ***
template<class T>
inline void scan::StdUtil::errorf(const string &t_msg, const T &t_arg)
{
    if (t_msg.find('%') == string::npos)
    {
        throw ArgEx("t_msg", "Missing format character: '%'");
    }
    print(FgColor::red, Util::fstr(t_msg, t_arg));
}

/// ***
/// Write general information to standard output
/// ***
template<class T>
inline void scan::StdUtil::printf(const string &t_msg, const T &t_arg)
{
    if (t_msg.find('%') == string::npos)
    {
        throw ArgEx("t_msg", "Missing format character: '%'");
    }
    print(FgColor::cyan, Util::fstr(t_msg, t_arg));
}

#endif // !STD_UTIL_H
