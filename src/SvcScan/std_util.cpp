/*
* @file
*     std_util.cpp
* @brief
*     Source file for console and standard console stream utilities.
*/
#include <windows.h>
#include "includes/errors/logic_ex.h"
#include "includes/io/std_util.h"

/**
* @brief
*     Virtual terminal sequence processing is enabled.
*/
std::atomic_bool scan::StdUtil::vt_enabled{ false };

/**
* @brief
*     Standard console error stream mutex.
*/
std::mutex scan::StdUtil::m_cerr_mtx;

/**
* @brief
*     Standard console output stream mutex.
*/
std::mutex scan::StdUtil::m_cout_mtx;

/**
* @brief  Set the title of the current console window.
*/
void scan::StdUtil::console_title(const string &t_title)
{
    if (!vt_enabled)
    {
        throw LogicEx{ "StdUtil::console_title", "VT sequences must be enabled" };
    }
    std::cout << algo::fstr("\033]0;%\x07", t_title);
}

/**
* @brief  Write the given error message to the standard error stream.
*         Locks the underlying standard error stream mutex.
*/
void scan::StdUtil::error(const string &t_msg)
{
    std::scoped_lock lock{ m_cerr_mtx };
    std::cerr << algo::fstr("% %%", colorize("[x]", Color::red), t_msg, LF);
}

/**
* @brief  Write the given exception message the to standard error stream.
*         Locks the underlying standard error stream mutex.
*/
void scan::StdUtil::except(const string &t_msg)
{
    std::scoped_lock lock{ m_cerr_mtx };
    std::cerr << algo::concat(LF, colorize(t_msg, Color::red), LF);
}

/**
* @brief  Write the given informational message to the standard output stream.
*         Locks the underlying standard output stream mutex.
*/
void scan::StdUtil::info(const string &t_msg)
{
    std::scoped_lock lock{ m_cout_mtx };
    std::cout << algo::fstr("% %%", colorize("[+]", Color::green), t_msg, LF);
}

/**
* @brief  Write the given status message to the standard output stream.
*         Locks the underlying standard output stream mutex.
*/
void scan::StdUtil::print(const string &t_msg)
{
    std::scoped_lock lock{ m_cout_mtx };
    std::cout << algo::fstr("% %%", colorize("[*]", Color::cyan), t_msg, LF);
}

/**
* @brief  Write the given warning message to the standard error stream.
*         Locks the underlying standard error stream mutex.
*/
void scan::StdUtil::warn(const string &t_msg)
{
    std::scoped_lock lock{ m_cerr_mtx };
    std::cerr << algo::fstr("% %%", colorize("[!]", Color::yellow), t_msg, LF);
}

/**
* @brief  Enable virtual terminal escape sequence processing.
*/
int scan::StdUtil::enable_vt()
{
    int rcode{ NO_ERROR };

    if (!vt_enabled)
    {
        ulong_t stdout_mode{ 0UL };
        HANDLE hstdout{ GetStdHandle(STD_OUTPUT_HANDLE) };

        const bool valid_handle{ hstdout != INVALID_HANDLE_VALUE };

        // Failed to get stdout mode
        if (!valid_handle || !GetConsoleMode(hstdout, &stdout_mode))
        {
            rcode = GetLastError();
        }

        if (rcode == NO_ERROR)
        {
            stdout_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

            // Failed to set stdout mode
            if (!SetConsoleMode(hstdout, stdout_mode))
            {
                rcode = GetLastError();
            }
        }
        vt_enabled = rcode == NO_ERROR;
    }
    return rcode;
}

/**
* @brief  Colorize the given message using the specified console foreground color.
*/
std::string scan::StdUtil::colorize(const string &t_msg, const Color &t_fg_color)
{
    string colored_msg;
    const size_t orig_size{ t_msg.size() };

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
* @brief  Create a header title using the given title string. Optionally specify
*         the underline character and whether the results should be colorized.
*/
std::string scan::StdUtil::hdr_title(const string &t_title,
                                     const bool &t_colorize,
                                     const char &t_ln_char)
{
    string title_str{ t_title };
    const string ln_str{ algo::underline(title_str.size(), t_ln_char) };

    if (t_colorize)
    {
        title_str = colorize(title_str, Color::green);
    }
    return algo::concat(title_str, LF, ln_str, LF);
}

/**
* @brief  Colorize the given message using the specified ANSI
*         foreground color sequence.
*/
std::string scan::StdUtil::colorize(const string &t_msg, const string &t_fg_color)
{
    return vt_enabled ? algo::concat(t_fg_color, t_msg, RESET) : t_msg;
}
