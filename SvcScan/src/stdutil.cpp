/*
*  stdutil.h
*  ---------
*  Source file for standard stream utilities
*/
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include "includes/io/stdutil.h"

bool scan::StdUtil::vt_enabled{ false };

/// ***
/// Write an error message to standard error
/// ***
void scan::StdUtil::error(const string &t_msg)
{
    std::cerr << color_str(RED, "[x]") << " " << t_msg << LF;
}

/// ***
/// Write exception information to standard error
/// ***
void scan::StdUtil::except(const ArgEx &t_ex)
{
    std::cerr << LF << color_str(RED, t_ex) << LF;
}

/// ***
/// Write exception information to standard error
/// ***
void scan::StdUtil::except(const LogicEx &t_ex)
{
    std::cerr << LF << color_str(RED, t_ex) << LF;
}

/// ***
/// Write status information to standard output
/// ***
void scan::StdUtil::info(const string &t_msg)
{
    std::cout << color_str(GREEN, "[+]") << " " << t_msg << LF;
}

/// ***
/// Write general information to standard output
/// ***
void scan::StdUtil::print(const string &t_msg)
{
    std::cout << color_str(CYAN, "[*]") << " " << t_msg << LF;
}

/// ***
/// Write a warning message to standard error
/// ***
void scan::StdUtil::warn(const string &t_msg)
{
    std::cerr << color_str(YELLOW, "[!]") << " " << t_msg << LF;
}

/// ***
/// Enable virtual terminal escape sequence processing
/// ***
int scan::StdUtil::enable_vt()
{
    int rcode{ NO_ERROR };

    if (!vt_enabled)
    {
        ulong stdout_mode{ NULL };
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
std::string scan::StdUtil::color_str(const string &t_fg, const string &t_msg)
{
    return vt_enabled ? Util::fstr("%%%", t_fg, t_msg, RESET) : t_msg;
}
