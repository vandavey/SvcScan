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
#include <cerrno>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include <consoleapi.h>
#include <consoleapi2.h>
#include <errhandlingapi.h>
#include <handleapi.h>
#include <processenv.h>
#include "includes/console/util.h"
#include "includes/errors/error_const_defs.h"
#include "includes/errors/logic_ex.h"
#include "includes/errors/runtime_ex.h"

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
        throw LogicEx{VT_DISABLED_MSG, "util::console_title"};
    }
    std::cout << algo::fstr("\x1b]0;%\x07", t_title);
}

/**
* @brief
*     Write the given error message to the standard error
*     stream. Locks the underlying standard error stream mutex.
*/
void scan::util::error(const string& t_msg)
{
    scoped_lock lock{cerr_mtx};
    std::cerr << algo::fstr("% %%", colorize("[x]", Color::red), t_msg, LF);
}

/**
* @brief
*     Write the given informational message to the standard output
*     stream. Locks the underlying standard output stream mutex.
*/
void scan::util::info(const string& t_msg)
{
    scoped_lock lock{cout_mtx};
    std::cout << algo::fstr("% %%", colorize("[+]", Color::green), t_msg, LF);
}

/**
* @brief
*     Customize the console title and enable virtual terminal processing.
*/
void scan::util::setup_console()
{
    const int rcode{enable_vt_processing()};

    if (rcode == RCODE_NO_ERROR)
    {
        console_title(app_title());
    }
    else  // Print enable failure warning
    {
        warn(VT_FAILED_MSG);
    }
}

/**
* @brief
*     Write the given warning message to the standard error
*     stream. Locks the underlying standard error stream mutex.
*/
void scan::util::warn(const string& t_msg)
{
    scoped_lock lock{cerr_mtx};
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
*     Get the current console window width.
*/
uint16_t scan::util::console_width()
{
    CONSOLE_SCREEN_BUFFER_INFO buffer_info{};
    HANDLE hstdout{GetStdHandle(STD_OUTPUT_HANDLE)};

    const bool valid_handle{hstdout != INVALID_HANDLE_VALUE};

    if (!valid_handle || !GetConsoleScreenBufferInfo(hstdout, &buffer_info))
    {
        const string error_msg{algo::fstr(CONSOLE_API_FAILED_FMT_MSG, GetLastError())};
        throw RuntimeEx{error_msg, "util::console_width"};
    }

    int16_t left_pos{buffer_info.srWindow.Left};
    int16_t right_pos{buffer_info.srWindow.Right};

    return static_cast<uint16_t>((right_pos - left_pos) + 1_i16);
}

#ifdef _DEBUG
/**
* @brief
*     Display the console debugger exit banner and read a single
*     key-press from the standard input stream. Blocks until a key-press
*     is detected if a key-press has not already been registered.
*/
int scan::util::debug_exit_read_key()
{
    print(DEBUG_EXIT_BANNER);
    return read_key();
}
#endif // _DEBUG

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
*     Read a single key-press from the standard input stream. Blocks until a
*     key-press is detected if a key-press has not already been registered.
*/
int scan::util::read_key()
{
    return _getch();
}

/**
* @brief
*     Get the value of the environment variable matching the given variable name.
*/
std::string scan::util::env_variable(const string& t_name)
{
    string value;

    if (!t_name.empty())
    {
        size_t var_size{0_sz};
        errno_t t_ecode{getenv_s(&var_size, nullptr, 0_sz, &t_name[0])};

        if (t_ecode == RCODE_NO_ERROR && var_size > 0)
        {
            value.reserve(var_size);
            value.resize(var_size - 1_sz);

            t_ecode = getenv_s(&var_size, &value[0], var_size, &t_name[0]);

            // Clear null allocations on failure
            if (t_ecode != RCODE_NO_ERROR)
            {
                value.clear();
            }
        }
    }
    return value;
}
