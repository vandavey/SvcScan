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
#include <mutex>
#include <string>
#include <utility>
#include "../concepts/concepts.h"
#include "../errors/exception.h"
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
        /// @brief  Cyan foreground color ANSI SGR control sequence.
        constexpr c_string_t CYAN = "\x1b[38;2;0;255;255m";

        /// @brief  Console debug exit banner.
        constexpr c_string_t DEBUG_EXIT_BANNER = "[DEBUG]: Press any key to terminate...";

        /// @brief  Green foreground color ANSI SGR control sequence.
        constexpr c_string_t GREEN = "\x1b[38;2;166;226;46m";

        /// @brief  Red foreground color ANSI SGR control sequence.
        constexpr c_string_t RED = "\x1b[38;2;246;0;0m";

        /// @brief  Style reset ANSI SGR control sequence.
        constexpr c_string_t RESET = "\x1b[0m";

        /// @brief  Yellow foreground color ANSI SGR control sequence.
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
    *     Colorize the given message using the specified
    *     ANSI foreground color SGR control sequence.
    */
    constexpr string colorize(const string& t_msg, const string& t_fg_color)
    {
        return vt_processing_enabled ? algo::concat(t_fg_color, t_msg, RESET) : t_msg;
    }

    /**
    * @brief
    *     Colorize the given message using the specified console foreground color.
    */
    constexpr string colorize(const string& t_msg, Color t_fg_color)
    {
        string colored_msg;

        switch (t_fg_color)
        {
            case Color::cyan:
                colored_msg = colorize(t_msg, CYAN);
                break;
            case Color::green:
                colored_msg = colorize(t_msg, GREEN);
                break;
            case Color::red:
                colored_msg = colorize(t_msg, RED);
                break;
            case Color::yellow:
                colored_msg = colorize(t_msg, YELLOW);
                break;
            default:
                colored_msg = colorize(t_msg, RESET);
                break;
        }
        return colored_msg;
    }

    /**
    * @brief
    *     Create a formatted field using the given label and value.
    *     Optionally specify whether the results should be colorized.
    */
    constexpr string fmt_field(const string& t_label,
                               const string& t_value,
                               bool t_colorize = false)
    {
        string label{t_label};

        if (t_colorize)
        {
            label = colorize(label, Color::green);
        }
        string field;

        if (t_value.empty())
        {
            field = algo::fstr("% :", label);
        }
        else  // Include value
        {
            field = algo::fstr("% : %", label, t_value);
        }
        return field;
    }

    /**
    * @brief
    *     Create a formatted field using the given label. Optionally
    *     specify whether the results should be colorized.
    */
    constexpr string fmt_field(const string& t_label, bool t_colorize = false)
    {
        return fmt_field(t_label, {}, t_colorize);
    }

    /**
    * @brief
    *     Create a formatted title using the given label. Optionally specify
    *     the underline character and whether the results should be colorized.
    */
    constexpr string fmt_title(const string& t_label,
                               bool t_colorize = false,
                               char t_ln_char = '=')
    {
        string title{t_label};
        const size_t ln_size{title.size()};

        if (t_colorize)
        {
            title = colorize(title, Color::green);
        }
        return algo::concat(title, LF, algo::underline(ln_size, t_ln_char));
    }

    /**
    * @brief
    *     Create a formatted title using the given label and value. Optionally specify
    *     the underline character and whether the results should be colorized.
    */
    constexpr string fmt_title(const string& t_label,
                               const string& t_value,
                               bool t_colorize = false,
                               char t_ln_char = '=')
    {
        const size_t delim_size{t_value.empty() ? 2_sz : 3_sz};
        const size_t ln_size{t_label.size() + delim_size + t_value.size()};
        const string title{fmt_field(t_label, t_value, t_colorize)};

        return algo::concat(title, LF, algo::underline(ln_size, t_ln_char));
    }

    void clear_keys();
    void console_title(const string& t_title);
    void error(const string& t_msg);

    template<LShift... ArgsT>
        requires AtLeastOne<ArgsT...>
    void errorf(const string& t_msg, ArgsT&&... t_args);

    void except(const Derived<Exception> auto& t_ex);
    void info(const string& t_msg);
    void print(const LShift auto& t_msg);

    template<LShift... ArgsT>
        requires AtLeastOne<ArgsT...>
    void printf(const string& t_msg, ArgsT&&... t_args);

    void setup_console();
    void warn(const string& t_msg);

    bool key_pressed();

    uint16_t console_width();

#ifdef _DEBUG
    int debug_exit_read_key();
#endif // _DEBUG

    int enable_vt_processing();
    int read_key();

    string env_variable(const string& t_name);
}

/**
* @brief
*     Interpolate arguments in the error message and
*     write the result to the standard error stream.
*/
template<scan::LShift... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline void scan::util::errorf(const string& t_msg, ArgsT&&... t_args)
{
    error(algo::fstr(t_msg, std::forward<ArgsT>(t_args)...));
}

/**
* @brief
*     Write the details of the given exception to the standard error
*     stream. Locks the underlying standard error stream mutex.
*/
inline void scan::util::except(const Derived<Exception> auto& t_ex)
{
    scoped_lock lock{cerr_mtx};
    std::cerr << algo::concat(LF, colorize(t_ex, Color::red), LF);
}

/**
* @brief
*     Write the given status message to the standard output
*     stream. Locks the underlying standard output stream mutex.
*/
inline void scan::util::print(const LShift auto& t_msg)
{
    scoped_lock lock{cout_mtx};
    std::cout << algo::fstr("% %%", colorize("[*]", Color::cyan), t_msg, LF);
}

/**
* @brief
*     Interpolate arguments in the status message and
*     write the result to the standard output stream.
*/
template<scan::LShift... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline void scan::util::printf(const string& t_msg, ArgsT&&... t_args)
{
    print(algo::fstr(t_msg, std::forward<ArgsT>(t_args)...));
}

#endif // !SCAN_UTIL_H
