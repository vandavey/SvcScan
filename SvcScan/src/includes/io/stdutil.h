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
#include "../except/argex.h"
#include "../except/logicex.h"
#include "../utils/util.h"

namespace scan
{
    /// ***
    /// Standard stream manipulation utilities
    /// ***
    class StdUtil final
    {
    private:  /* Types */
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
        static void print_color(ostream &t_os,
                                const string &t_fg,
                                const string &t_msg);

        static void print_status(ostream &t_os,
                                 const string &t_fg,
                                 const string &t_symbol,
                                 const string &t_msg);
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
    print_status(std::cerr, RED, "[x]", Util::fstr(t_msg, t_arg));
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
    print_status(std::cout, CYAN, "[*]", Util::fstr(t_msg, t_arg));
}

#endif // !STD_UTIL_H
