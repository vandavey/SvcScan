/*
* @file
*     std_util.h
* @brief
*     Header file for console and standard console stream utilities.
*/
#pragma once

#ifndef SCAN_STD_UTIL_H
#define SCAN_STD_UTIL_H

#include <iostream>
#include <string>
#include "../concepts/concepts.h"
#include "../threading/thread_defs.h"
#include "../utils/algorithm.h"
#include "../utils/expr.h"
#include "../utils/type_defs.h"
#include "color.h"

namespace scan
{
    /**
    * @brief
    *     Console and standard console stream utilities.
    */
    class StdUtil final
    {
    private:  /* Constants */
        static constexpr cstr_t RESET = "\033[0m";  // Ansi reset sequence

        // Ansi foreground color control sequences
        static constexpr cstr_t RED    = "\033[38;2;246;0;0m";
        static constexpr cstr_t CYAN   = "\033[38;2;0;255;255m";
        static constexpr cstr_t GREEN  = "\033[38;2;166;226;46m";
        static constexpr cstr_t YELLOW = "\033[38;2;250;230;39m";

    public:  /* Fields */
        static atomic_bool vt_enabled;  // VT control sequence processing

    private:  /* Fields */
        static mutex m_cerr_mtx;  // Standard error mutex
        static mutex m_cout_mtx;  // Standard output mutex

    public:  /* Constructors & Destructor */
        StdUtil() = delete;
        StdUtil(const StdUtil &) = delete;
        StdUtil(StdUtil &&) = delete;

        virtual ~StdUtil() = default;

    public:  /* Operators */
        StdUtil &operator=(const StdUtil &) = default;
        StdUtil &operator=(StdUtil &&) = default;

    public:  /* Methods */
        static void console_title(const string &t_title);
        static void error(const string &t_msg);

        template<LShift ...ArgsT>
        static void errorf(const string &t_msg, const ArgsT &...t_args);

        static void except(const string &t_msg);
        static void info(const string &t_msg);

        template<LShift T>
        static void print(const T &t_msg);

        template<LShift ...ArgsT>
        static void printf(const string &t_msg, const ArgsT &...t_args);

        static void warn(const string &t_msg);

        template<LShift ...ArgsT>
        static void warnf(const string &t_msg, const ArgsT &...t_args);

        static int enable_vt();

        static string colorize(const string &t_msg, const Color &t_fg_color);

        static string header_title(const string &t_title,
                                   const bool &t_colorize = false,
                                   const char &t_ln_char = '=');

        template<LShift T>
        static string header_title(const string &t_title_label,
                                   const T &t_title_value,
                                   const bool &t_colorize = false,
                                   const char &t_ln_char = '=');

        template<LShift T>
        static string title(const string &t_title_label,
                            const T &t_title_value,
                            const bool &t_colorize = false);

        template<LShift T>
        static string title(const string &t_title_label,
                            const T &t_title_value,
                            const bool &t_colorize,
                            size_t &t_ln_size);

    private:  /* Methods */
        static string colorize(const string &t_msg, const string &t_fg_color);
    };
}

/**
* @brief
*     Interpolate arguments in the error message and
*     write the result to the standard error stream.
*/
template<scan::LShift ...ArgsT>
inline void scan::StdUtil::errorf(const string &t_msg, const ArgsT &...t_args)
{
    static_assert(sizeof...(t_args) > 0);
    error(algo::fstr(t_msg, t_args...));
}

/**
* @brief
*     Write the given status message to the standard output
*     stream. Locks the underlying standard output stream mutex.
*/
template<scan::LShift T>
inline void scan::StdUtil::print(const T &t_msg)
{
    std::scoped_lock lock{ m_cout_mtx };
    std::cout << algo::fstr("% %%", colorize("[*]", Color::cyan), t_msg, LF);
}

/**
* @brief
*     Interpolate arguments in the status message and
*     write the result to the standard output stream.
*/
template<scan::LShift ...ArgsT>
inline void scan::StdUtil::printf(const string &t_msg, const ArgsT &...t_args)
{
    static_assert(sizeof...(t_args) > 0);
    print(algo::fstr(t_msg, t_args...));
}

/**
* @brief
*     Interpolate arguments in the warning message and
*     write the result to the standard error stream.
*/
template<scan::LShift ...ArgsT>
inline void scan::StdUtil::warnf(const string &t_msg, const ArgsT &...t_args)
{
    static_assert(sizeof...(t_args) > 0);
    warn(algo::fstr(t_msg, t_args...));
}

/**
* @brief
*     Create a header title using the given label and value. Optionally specify
*     the underline character and whether the results should be colorized.
*/
template<scan::LShift T>
inline std::string scan::StdUtil::header_title(const string &t_title_label,
                                               const T &t_title_value,
                                               const bool &t_colorize,
                                               const char &t_ln_char)
{
    size_t ln_size{ 0 };
    const string title_str{ title(t_title_label, t_title_value, t_colorize, ln_size) };

    return algo::concat(title_str, LF, algo::underline(ln_size, t_ln_char));
}

/**
* @brief
*     Create a title using the given label and value. Optionally
*     specify whether the results should be colorized.
*/
template<scan::LShift T>
inline std::string scan::StdUtil::title(const string &t_title_label,
                                        const T &t_title_value,
                                        const bool &t_colorize)
{
    size_t ln_size{ 0 };
    return title(t_title_label, t_title_value, t_colorize, ln_size);
}

/**
* @brief
*     Create a title using the given label and value. Optionally specify whether
*     the results should be colorized and the uncolored size reference.
*/
template<scan::LShift T>
inline std::string scan::StdUtil::title(const string &t_title_label,
                                        const T &t_title_value,
                                        const bool &t_colorize,
                                        size_t &t_ln_size)
{
    string new_label{ t_title_label };
    const string new_value{ algo::fstr(" : %", t_title_value) };

    t_ln_size = new_label.size() + new_value.size();

    if (t_colorize)
    {
        new_label = colorize(new_label, Color::green);
    }
    return algo::concat(new_label, new_value);
}

#endif // !SCAN_STD_UTIL_H
