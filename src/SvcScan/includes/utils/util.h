/*
* @file
*     util.h
* @brief
*     Header file for console and standard console stream utilities.
*/
#pragma once

#ifndef SCAN_UTIL_H
#define SCAN_UTIL_H

#include <iostream>
#include <string>
#include "../concepts/concepts.h"
#include "../io/color.h"
#include "../threading/thread_aliases.h"
#include "algo.h"
#include "aliases.h"
#include "const_defs.h"
#include "literals.h"

/**
* @brief
*     Console and standard console stream utilities.
*/
namespace scan::util
{
    /**
    * @brief
    *     Console and standard console stream constant fields.
    */
    inline namespace defs
    {
        /// @brief  Cyan foreground ANSI control sequence.
        constexpr cstr_t CYAN = "\033[38;2;0;255;255m";

        /// @brief  Green foreground ANSI control sequence.
        constexpr cstr_t GREEN = "\033[38;2;166;226;46m";

        /// @brief  Red foreground ANSI control sequence.
        constexpr cstr_t RED = "\033[38;2;246;0;0m";

        /// @brief  Reset ANSI control sequence.
        constexpr cstr_t RESET = "\033[0m";

        /// @brief  Yellow foreground ANSI control sequence.
        constexpr cstr_t YELLOW = "\033[38;2;250;230;39m";
    }

    /// @brief  Virtual terminal sequence processing is enabled.
    inline atomic_bool vt_processing_enabled{ false };

    /// @brief  Standard console error stream mutex.
    inline mutex cerr_mtx{};

    /// @brief  Standard console output stream mutex.
    inline mutex cout_mtx{};

    /**
    * @brief
    *     Colorize the given message using the specified ANSI foreground color sequence.
    */
    constexpr string colorize(const string& t_msg, const string& t_fg_color)
    {
        return vt_processing_enabled ? algo::concat(t_fg_color, t_msg, RESET) : t_msg;
    }

    void console_title(const string& t_title);
    void error(const string& t_msg);

    template<LShift... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    void errorf(const string& t_msg, const ArgsT&... t_args);

    void except(const string& t_msg);
    void info(const string& t_msg);

    template<LShift T>
    void print(const T& t_msg);

    template<LShift... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    void printf(const string& t_msg, const ArgsT&... t_args);

    void warn(const string& t_msg);

    template<LShift... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    void warnf(const string& t_msg, const ArgsT&... t_args);

    int enable_vt_processing();

    string colorize(const string& t_msg, const Color& t_fg_color);

    string header_title(const string& t_title,
                        const bool& t_colorize = false,
                        const char& t_ln_char = '=');

    template<LShift T>
    string header_title(const string& t_title_label,
                        const T& t_title_value,
                        const bool& t_colorize = false,
                        const char& t_ln_char = '=');

    template<LShift T>
    string title(const string& t_title_label,
                 const T& t_title_value,
                 const bool& t_colorize = false);

    template<LShift T>
    string title(const string& t_title_label,
                 const T& t_title_value,
                 const bool& t_colorize,
                 size_t& t_ln_size);
}

/**
* @brief
*     Interpolate arguments in the error message and
*     write the result to the standard error stream.
*/
template<scan::LShift... ArgsT>
    requires scan::AtLeastOneParam<ArgsT...>
inline void scan::util::errorf(const string& t_msg, const ArgsT&... t_args)
{
    error(algo::fstr(t_msg, t_args...));
}

/**
* @brief
*     Write the given status message to the standard output
*     stream. Locks the underlying standard output stream mutex.
*/
template<scan::LShift T>
inline void scan::util::print(const T& t_msg)
{
    std::scoped_lock lock{ cout_mtx };
    std::cout << algo::fstr("% %%", colorize("[*]", Color::cyan), t_msg, LF);
}

/**
* @brief
*     Interpolate arguments in the status message and
*     write the result to the standard output stream.
*/
template<scan::LShift... ArgsT>
    requires scan::AtLeastOneParam<ArgsT...>
inline void scan::util::printf(const string& t_msg, const ArgsT&... t_args)
{
    print(algo::fstr(t_msg, t_args...));
}

/**
* @brief
*     Interpolate arguments in the warning message and
*     write the result to the standard error stream.
*/
template<scan::LShift... ArgsT>
    requires scan::AtLeastOneParam<ArgsT...>
inline void scan::util::warnf(const string& t_msg, const ArgsT&... t_args)
{
    warn(algo::fstr(t_msg, t_args...));
}

/**
* @brief
*     Create a header title using the given label and value. Optionally specify
*     the underline character and whether the results should be colorized.
*/
template<scan::LShift T>
inline std::string scan::util::header_title(const string& t_title_label,
                                            const T& t_title_value,
                                            const bool& t_colorize,
                                            const char& t_ln_char)
{
    size_t ln_size{ 0_st };
    const string title_str{ title(t_title_label, t_title_value, t_colorize, ln_size) };

    return algo::concat(title_str, LF, algo::underline(ln_size, t_ln_char));
}

/**
* @brief
*     Create a title using the given label and value. Optionally
*     specify whether the results should be colorized.
*/
template<scan::LShift T>
inline std::string scan::util::title(const string& t_title_label,
                                     const T& t_title_value,
                                     const bool& t_colorize)
{
    size_t ln_size{ 0_st };
    return title(t_title_label, t_title_value, t_colorize, ln_size);
}

/**
* @brief
*     Create a title using the given label and value. Optionally specify whether
*     the results should be colorized and the uncolored size reference.
*/
template<scan::LShift T>
inline std::string scan::util::title(const string& t_title_label,
                                     const T& t_title_value,
                                     const bool& t_colorize,
                                     size_t& t_ln_size)
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

#endif // !SCAN_UTIL_H
