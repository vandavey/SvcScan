/*
*  std_util.cpp
*  ------------
*  Source file for standard stream utilities
*/
#include <iostream>
#include <windows.h>
#include "includes/io/std_util.h"

bool scan::StdUtil::vt_enabled{ false };

/// ***
/// Write an error message to standard error
/// ***
void scan::StdUtil::error(const string &t_msg)
{
    std::cerr << Util::fstr("% %%", str_color(RED, "[x]"), t_msg, LF);
}

/// ***
/// Write the given exception message to standard error
/// ***
void scan::StdUtil::except(const string &t_ex_msg)
{
    std::cerr << Util::fstr("%%%", LF, str_color(RED, t_ex_msg), LF);
}

/// ***
/// Write the given status information to standard output
/// ***
void scan::StdUtil::info(const string &t_msg)
{
    std::cout << Util::fstr("% %%", str_color(GREEN, "[+]"), t_msg, LF);
}

/// ***
/// Write the given general information to standard output
/// ***
void scan::StdUtil::print(const string &t_msg)
{
    std::cout << Util::fstr("% %%", str_color(CYAN, "[*]"), t_msg, LF);
}

/// ***
/// Write the given warning message to standard error
/// ***
void scan::StdUtil::warn(const string &t_msg)
{
    std::cerr << Util::fstr("% %%", str_color(YELLOW, "[!]"), t_msg, LF);
}

/// ***
/// Enable virtual terminal escape sequence processing
/// ***
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

        // Set the stdout mode
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

/// ***
/// Format all contents of the given string in the specified foreground color
/// ***
std::string scan::StdUtil::str_color(const string &t_fg, const string &t_msg)
{
    return vt_enabled ? Util::fstr("%%%", t_fg, t_msg, RESET) : t_msg;
}
