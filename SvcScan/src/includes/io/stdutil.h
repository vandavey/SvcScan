/*
*  stdutil.h
*  ---------
*  Header file for standard stream utilities
*/
#pragma once

#ifndef STD_UTIL_H
#define STD_UTIL_H

#include <iostream>
#include <string>
#include "../utils/util.h"

namespace scan
{
    /// ***
    /// Standard stream manipulation utilities
    /// ***
    class StdUtil final
    {
    private:  /* Type Aliases */
        using ulong = unsigned long;

        using ostream = std::ostream;
        using string  = std::string;

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
        static bool vt_enabled;  // VT escape processing

    public:  /* Destructor */
        virtual ~StdUtil() = default;

    private:  /* Constructors (deleted) */
        StdUtil() = delete;
        StdUtil(const StdUtil &) = delete;

    public:  /* Methods */
        static void error(const string &t_msg);

        template<class T, class ...Args>
        static void errorf(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        static void except(const string &t_ex_msg);
        static void info(const string &t_msg);
        static void print(const string &t_msg);

        template<class T, class ...Args>
        static void printf(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        static void warn(const string &t_msg);

        template<class T, class ...Args>
        static void warnf(const string &t_msg,
                          const T &t_arg,
                          const Args &...t_args);

        static int enable_vt();

    private:  /* Methods */
        static string str_color(const string &t_fg, const string &t_msg);
    };
}

/// ***
/// Interpolate arguments in the error message and write the result to stderr
/// ***
template<class T, class ...Args>
inline void scan::StdUtil::errorf(const string &t_msg,
                                  const T &t_arg,
                                  const Args &...t_args) {

    error(Util::fstr(t_msg, t_arg, t_args...));
}

/// ***
/// Interpolate arguments in the message and write the result to stdout
/// ***
template<class T, class ...Args>
inline void scan::StdUtil::printf(const string &t_msg,
                                  const T &t_arg,
                                  const Args &...t_args) {

    print(Util::fstr(t_msg, t_arg, t_args...));
}

/// ***
/// Interpolate arguments in the warning message and write the result stderr
/// ***
template<class T, class ...Args>
inline void scan::StdUtil::warnf(const string &t_msg,
                                 const T &t_arg,
                                 const Args &...t_args) {

    warn(Util::fstr(t_msg, t_arg, t_args...));
}

#endif // !STD_UTIL_H
