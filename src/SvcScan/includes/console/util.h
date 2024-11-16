/*
* @file
*     util.h
* @brief
*     Header file for console and standard console stream utilities.
*/
#pragma once

#ifndef SCAN_UTIL_H
#define SCAN_UTIL_H

#include <cstdint>
#include <iostream>
#include <string>
#include "../concepts/concepts.h"
#include "../ranges/algo.h"
#include "../threading/thread_aliases.h"
#include "../utils/aliases.h"
#include "../utils/const_defs.h"
#include "../utils/literals.h"
#include "color.h"

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
        constexpr c_string_t CYAN = "\x1b[38;2;0;255;255m";

        /// @brief  Console debug exit banner.
        constexpr c_string_t DEBUG_EXIT_BANNER = "[DEBUG]: Press any key to terminate...";

        /// @brief  Green foreground ANSI control sequence.
        constexpr c_string_t GREEN = "\x1b[38;2;166;226;46m";

        /// @brief  Red foreground ANSI control sequence.
        constexpr c_string_t RED = "\x1b[38;2;246;0;0m";

        /// @brief  Reset ANSI control sequence.
        constexpr c_string_t RESET = "\x1b[0m";

        /// @brief  Yellow foreground ANSI control sequence.
        constexpr c_string_t YELLOW = "\x1b[38;2;250;230;39m";
    }

    /// @brief  Virtual terminal sequence processing is enabled.
    inline atomic_bool vt_processing_enabled{false};

    /// @brief  Standard console error stream mutex.
    inline mutex cerr_mtx{};

    /// @brief  Standard console output stream mutex.
    inline mutex cout_mtx{};

    /**
    * @brief
    *     Get the application name and repository formatted as a title.
    */
    constexpr string app_title()
    {
        return algo::fstr("% (%)", APP, REPO);
    }

    /**
    * @brief
    *     Get the application name and repository formatted as a title. Includes
    *     the specified subtitle between the application name and repository.
    */
    constexpr string app_title(const string& t_subtitle)
    {
        return algo::fstr("% - % (%)", APP, t_subtitle, REPO);
    }

    /**
    * @brief
    *     Colorize the given message using the specified ANSI foreground color sequence.
    */
    constexpr string colorize(const string& t_msg, const string& t_fg_color)
    {
        return vt_processing_enabled ? algo::concat(t_fg_color, t_msg, RESET) : t_msg;
    }

    void clear_keys();
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

    void setup_console();
    void warn(const string& t_msg);

    template<LShift... ArgsT>
        requires AtLeastOneParam<ArgsT...>
    void warnf(const string& t_msg, const ArgsT&... t_args);

    bool key_pressed();

    uint16_t console_width();

#ifdef _DEBUG
    int debug_exit_read_key();
#endif // _DEBUG

    int enable_vt_processing();
    int read_key();

    string colorize(const string& t_msg, Color t_fg_color);
    string env_variable(const string& t_name);

    template<LShift T>
    string fmt_field(const string& t_title_label,
                     const T& t_title_value,
                     bool t_colorize = false);

    template<LShift T>
    string fmt_field(const string& t_title_label,
                     const T& t_title_value,
                     bool t_colorize,
                     size_t& t_result_size);

    string fmt_title(const string& t_title,
                     bool t_colorize = false,
                     char t_ln_char = '=');

    template<LShift T>
    string fmt_title(const string& t_title_label,
                     const T& t_title_value,
                     bool t_colorize = false,
                     char t_ln_char = '=');
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
    std::scoped_lock lock{cout_mtx};
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
*     Create a formatted field using the given label and value.
*     Optionally specify whether the results should be colorized.
*/
template<scan::LShift T>
inline std::string scan::util::fmt_field(const string& t_title_label,
                                         const T& t_title_value,
                                         bool t_colorize)
{
    size_t result_size{0_st};
    return fmt_field(t_title_label, t_title_value, t_colorize, result_size);
}

/**
* @brief
*     Create a formatted field using the given label and value.
*     Optionally specify whether the results should be colorized.
*/
template<scan::LShift T>
inline std::string scan::util::fmt_field(const string& t_title_label,
                                         const T& t_title_value,
                                         bool t_colorize,
                                         size_t& t_result_size)
{
    string new_label{t_title_label};
    const string new_value{algo::fstr(" : %", t_title_value)};

    t_result_size = new_label.size() + new_value.size();

    if (t_colorize)
    {
        new_label = colorize(new_label, Color::green);
    }
    return algo::concat(new_label, new_value);
}

/**
* @brief
*     Create a formatted title using the given label and value. Optionally specify
*     the underline character and whether the results should be colorized.
*/
template<scan::LShift T>
inline std::string scan::util::fmt_title(const string& t_title_label,
                                         const T& t_title_value,
                                         bool t_colorize,
                                         char t_ln_char)
{
    size_t result_size{0_st};
    const string field{fmt_field(t_title_label, t_title_value, t_colorize, result_size)};

    return algo::concat(field, LF, algo::underline(result_size, t_ln_char));
}

#endif // !SCAN_UTIL_H
