/*
* @file
*     util.h
* @brief
*     Header file for console and standard console stream utilities.
*/
#pragma once

#ifndef SCAN_UTIL_H
#define SCAN_UTIL_H

#include <atomic>
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
#include "severity.h"

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
        /// @brief  Console debug exit banner.
        constexpr c_string_t DEBUG_EXIT_BANNER = "[DEBUG]: Press any key to terminate...";

        /// @brief  Error output severity prefix.
        constexpr c_string_t SEV_PREFIX_ERROR = "[x]";

        /// @brief  Informational output severity prefix.
        constexpr c_string_t SEV_PREFIX_INFO = "[*]";

        /// @brief  Successful output severity prefix.
        constexpr c_string_t SEV_PREFIX_SUCCESS = "[+]";

        /// @brief  Warning output severity prefix.
        constexpr c_string_t SEV_PREFIX_WARNING = "[!]";

        /// @brief  Cyan foreground color ANSI SGR control sequence.
        constexpr c_string_t SGR_FG_CYAN = "\x1b[38;2;0;255;255m";

        /// @brief  Green foreground color ANSI SGR control sequence.
        constexpr c_string_t SGR_FG_GREEN = "\x1b[38;2;166;226;46m";

        /// @brief  Red foreground color ANSI SGR control sequence.
        constexpr c_string_t SGR_FG_RED = "\x1b[38;2;246;0;0m";

        /// @brief  Yellow foreground color ANSI SGR control sequence.
        constexpr c_string_t SGR_FG_YELLOW = "\x1b[38;2;250;230;39m";

        /// @brief  Style reset ANSI SGR control sequence.
        constexpr c_string_t SGR_RESET = "\x1b[0m";
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
    template<LShift T>
    constexpr string app_title(T&& t_subtitle)
    {
        return algo::fstr("% - % (%)", APP, std::forward<T>(t_subtitle), REPO);
    }

    /**
    * @brief
    *     Colorize the given message using the specified
    *     ANSI foreground color SGR control sequence.
    */
    template<LShift T, StringLike ColorSeqT>
    constexpr string colorize(T&& t_msg, ColorSeqT&& t_fg_color_seq)
    {
        string colored_msg;

        if (vt_processing_enabled)
        {
            colored_msg = algo::concat(std::forward<ColorSeqT>(t_fg_color_seq),
                                       std::forward<T>(t_msg),
                                       SGR_RESET);
        }
        else  // Skip colorization
        {
            colored_msg = algo::to_string(std::forward<T>(t_msg));
        }
        return colored_msg;
    }

    /**
    * @brief
    *     Colorize the given message using the specified console foreground color.
    */
    template<LShift T>
    constexpr string colorize(T&& t_msg, Color t_fg_color)
    {
        string colored_msg;

        switch (t_fg_color)
        {
            case Color::cyan:
                colored_msg = colorize(std::forward<T>(t_msg), SGR_FG_CYAN);
                break;
            case Color::green:
                colored_msg = colorize(std::forward<T>(t_msg), SGR_FG_GREEN);
                break;
            case Color::red:
                colored_msg = colorize(std::forward<T>(t_msg), SGR_FG_RED);
                break;
            case Color::yellow:
                colored_msg = colorize(std::forward<T>(t_msg), SGR_FG_YELLOW);
                break;
            default:
                colored_msg = colorize(std::forward<T>(t_msg), SGR_RESET);
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

    /**
    * @brief
    *     Get the output severity prefix corresponding to the given severity level.
    */
    constexpr string severity_prefix(Severity t_severity)
    {
        string prefix;

        switch (t_severity)
        {
            case Severity::error:
                prefix = colorize(SEV_PREFIX_ERROR, Color::red);
                break;
            case Severity::warn:
                prefix = colorize(SEV_PREFIX_WARNING, Color::yellow);
                break;
            case Severity::success:
                prefix = colorize(SEV_PREFIX_SUCCESS, Color::green);
                break;
            default:
                prefix = colorize(SEV_PREFIX_INFO, Color::cyan);
                break;
        }
        return prefix;
    }

    void clear_keys();
    void console_title(const string& t_title);

    template<LShift... ArgsT>
    void errorf(const string& t_msg, ArgsT&&... t_args);

    void except(const DerivedFrom<Exception> auto& t_ex);

    template<LShift... ArgsT>
    void printf(const string& t_msg, ArgsT&&... t_args);

    template<LShift... ArgsT>
    void printf(Severity t_severity, const string& t_msg, ArgsT&&... t_args);

    void setup_console();

    template<LShift... ArgsT>
    void successf(const string& t_msg, ArgsT&&... t_args);

    template<LShift... ArgsT>
    void warnf(const string& t_msg, ArgsT&&... t_args);

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
*     Interpolate arguments in the given error message and write it to
*     the standard error stream. Locks the standard error stream mutex.
*/
template<scan::LShift... ArgsT>
inline void scan::util::errorf(const string& t_msg, ArgsT&&... t_args)
{
    printf(Severity::error, t_msg, std::forward<ArgsT>(t_args)...);
}

/**
* @brief
*     Write the details of the given exception to the standard
*     error stream. Locks the standard error stream mutex.
*/
inline void scan::util::except(const DerivedFrom<Exception> auto& t_ex)
{
    scoped_lock lock{cerr_mtx};
    std::cerr << algo::concat(LF, colorize(t_ex, Color::red), LF);
}

/**
* @brief
*     Interpolate arguments in the given informational message and write it
*     to the standard output stream. Locks the standard output stream mutex.
*/
template<scan::LShift... ArgsT>
inline void scan::util::printf(const string& t_msg, ArgsT&&... t_args)
{
    printf(Severity::info, t_msg, std::forward<ArgsT>(t_args)...);
}

/**
* @brief
*     Interpolate arguments in the given status message and write
*     the result to the standard output or standard error stream.
*     Locks the corresponding output stream mutex.
*/
template<scan::LShift... ArgsT>
inline void scan::util::printf(Severity t_severity,
                               const string& t_msg,
                               ArgsT&&... t_args)
{
    string msg;

    // Interpolate message arguments
    if constexpr (AtLeastOne<ArgsT...>)
    {
        msg = algo::fstr(t_msg, std::forward<ArgsT>(t_args)...);
    }
    else  // Interpolation unnecessary
    {
        msg = t_msg;
    }

    msg = algo::fstr("% %%", severity_prefix(t_severity), std::move(msg), LF);

    // Print to standard output
    if (algo::any_equal(t_severity, Severity::info, Severity::success))
    {
        scoped_lock lock{cout_mtx};
        std::cout << std::move(msg);
    }
    else  // Print to standard error
    {
        scoped_lock lock{cerr_mtx};
        std::cerr << std::move(msg);
    }
}

/**
* @brief
*     Interpolate arguments in the given success message and write the result to
*     the standard output stream. Locks the standard output stream mutex.
*/
template<scan::LShift... ArgsT>
inline void scan::util::successf(const string& t_msg, ArgsT&&... t_args)
{
    printf(Severity::success, t_msg, std::forward<ArgsT>(t_args)...);
}

/**
* @brief
*     Interpolate arguments in the given warning message and write it to
*     the standard error stream. Locks the standard error stream mutex.
*/
template<scan::LShift... ArgsT>
inline void scan::util::warnf(const string& t_msg, ArgsT&&... t_args)
{
    printf(Severity::warn, t_msg, std::forward<ArgsT>(t_args)...);
}

#endif // !SCAN_UTIL_H
