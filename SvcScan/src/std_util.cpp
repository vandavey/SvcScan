/*
*  std_util.cpp
*  ------------
*  Source file for standard console stream utilities
*/
#include <iostream>
#include <windows.h>
#include "includes/io/std_util.h"

/**
* @brief  Virtual terminal sequence processing is enabled.
*/
std::atomic_bool scan::StdUtil::vt_enabled{ false };

/**
* @brief  Standard console error stream mutex.
*/
std::mutex scan::StdUtil::m_cerr_mtx;

/**
* @brief  Standard console output stream mutex.
*/
std::mutex scan::StdUtil::m_cout_mtx;

/**
* @brief  Write the given error message to the standard error stream.
*         Locks the underlying standard error stream mutex.
*/
void scan::StdUtil::error(const string &t_msg)
{
    scoped_lock lock{ m_cerr_mtx };
    std::cerr << algo::fstr("% %%", str_color(RED, "[x]"), t_msg, LF);
}

/**
* @brief  Write the given exception message the to standard error stream.
*         Locks the underlying standard error stream mutex.
*/
void scan::StdUtil::except(const string &t_msg)
{
    scoped_lock lock{ m_cerr_mtx };
    std::cerr << algo::fstr("%%%", LF, str_color(RED, t_msg), LF);
}

/**
* @brief  Write the given informational message to the standard output stream.
*         Locks the underlying standard output stream mutex.
*/
void scan::StdUtil::info(const string &t_msg)
{
    scoped_lock lock{ m_cout_mtx };
    std::cout << algo::fstr("% %%", str_color(GREEN, "[+]"), t_msg, LF);
}

/**
* @brief  Write the given status message to the standard output stream.
*         Locks the underlying standard output stream mutex.
*/
void scan::StdUtil::print(const string &t_msg)
{
    scoped_lock lock{ m_cout_mtx };
    std::cout << algo::fstr("% %%", str_color(CYAN, "[*]"), t_msg, LF);
}

/**
* @brief  Write the given warning message to the standard error stream.
*         Locks the underlying standard error stream mutex.
*/
void scan::StdUtil::warn(const string &t_msg)
{
    scoped_lock lock{ m_cerr_mtx };
    std::cerr << algo::fstr("% %%", str_color(YELLOW, "[!]"), t_msg, LF);
}

/**
* @brief  Enable virtual terminal escape sequence processing.
*/
int scan::StdUtil::enable_vt()
{
    int rcode{ NO_ERROR };

    if (!vt_enabled)
    {
        ulong stdout_mode{ 0UL };
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
* @brief  Format the given string value in the specified foreground color sequence.
*/
std::string scan::StdUtil::str_color(const string &t_fg, const string &t_msg)
{
    return vt_enabled ? algo::fstr("%%%", t_fg, t_msg, RESET) : t_msg;
}
