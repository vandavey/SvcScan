/*
* @file
*     util.cpp
* @brief
*     Source file for console and standard console stream utilities.
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <atomic>
#include <conio.h>
#include <windows.h>
#include <consoleapi.h>
#include <errhandlingapi.h>
#include <handleapi.h>
#include <processenv.h>
#include "includes/errors/logic_ex.h"
#include "includes/utils/util.h"

/**
* @brief
*     Clear all key-presses from the standard input stream.
*/
void scan::util::clear_keys()
{
    while (key_pressed())
    {
        read_key();
    }
}

/**
* @brief
*     Set the title of the current console window.
*/
void scan::util::console_title(const string& t_title)
{
    if (!vt_processing_enabled)
    {
        throw LogicEx{"util::console_title", "VT sequences must be enabled"};
    }
    std::cout << algo::fstr("\033]0;%\x07", t_title);
}

/**
* @brief
*     Write the given error message to the standard error
*     stream. Locks the underlying standard error stream mutex.
*/
void scan::util::error(const string& t_msg)
{
    std::scoped_lock lock{cerr_mtx};
    std::cerr << algo::fstr("% %%", colorize("[x]", Color::red), t_msg, LF);
}

/**
* @brief
*     Write the given exception message the to the standard error
*     stream. Locks the underlying standard error stream mutex.
*/
void scan::util::except(const string& t_msg)
{
    std::scoped_lock lock{cerr_mtx};
    std::cerr << algo::concat(LF, colorize(t_msg, Color::red), LF);
}

/**
* @brief
*     Write the given informational message to the standard output
*     stream. Locks the underlying standard output stream mutex.
*/
void scan::util::info(const string& t_msg)
{
    std::scoped_lock lock{cout_mtx};
    std::cout << algo::fstr("% %%", colorize("[+]", Color::green), t_msg, LF);
}

/**
* @brief
*     Write the given warning message to the standard error
*     stream. Locks the underlying standard error stream mutex.
*/
void scan::util::warn(const string& t_msg)
{
    std::scoped_lock lock{cerr_mtx};
    std::cerr << algo::fstr("% %%", colorize("[!]", Color::yellow), t_msg, LF);
}

/**
* @brief
*     Determine whether a key-press was detected.
*/
bool scan::util::key_pressed()
{
    return static_cast<bool>(_kbhit());
}

/**
* @brief
*     Enable virtual terminal control sequence processing.
*/
int scan::util::enable_vt_processing()
{
    int rcode{NO_ERROR};

    if (!vt_processing_enabled)
    {
        ulong_t stdout_mode{0UL};
        HANDLE hstdout{GetStdHandle(STD_OUTPUT_HANDLE)};

        const bool valid_handle{hstdout != INVALID_HANDLE_VALUE};

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
        vt_processing_enabled = rcode == NO_ERROR;
    }
    return rcode;
}

/**
* @brief
*     Read a single key-press from the standard input stream. Blocks until
*     a key-press is detected if a key-press has not already been detected.
*/
int scan::util::read_key()
{
    return _getch();
}

/**
* @brief
*     Colorize the given message using the specified console foreground color.
*/
std::string scan::util::colorize(const string& t_msg, Color t_fg_color)
{
    string colored_msg;
    const size_t orig_size{t_msg.size()};

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
*     Create a formatted title using the given title string. Optionally specify
*     the underline character and whether the results should be colorized.
*/
std::string scan::util::fmt_title(const string& t_title, bool t_colorize, char t_ln_char)
{
    string title_str{t_title};
    const string ln_str{algo::underline(title_str.size(), t_ln_char)};

    if (t_colorize)
    {
        title_str = colorize(title_str, Color::green);
    }
    return algo::concat(title_str, LF, ln_str);
}
