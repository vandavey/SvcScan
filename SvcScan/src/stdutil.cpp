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
#include "includes/conio/stdutil.h"

scan::AutoProp<bool> scan::StdUtil::vt_enabled{ false };

scan::StdUtil::map_cs scan::StdUtil::m_icons
{
    { FgColor::cyan,   "[*]" },
    { FgColor::yellow, "[!]" },
    { FgColor::red,    "[x]" }
};

/// ***
/// Write an error message to standard error
/// ***
void scan::StdUtil::error(const string &t_msg)
{
    print(FgColor::red, t_msg);
}

/// ***
/// Write exception information to standard error
/// ***
void scan::StdUtil::except(const ArgEx &t_ex)
{
    error(static_cast<string>(t_ex));
    std::cerr << t_ex << LF;
}

/// ***
/// Write exception information to standard error
/// ***
void scan::StdUtil::except(const LogicEx &t_ex)
{
    error(static_cast<string>(t_ex));
    std::cerr << t_ex << LF;
}

/// ***
/// Write general information to standard output
/// ***
void scan::StdUtil::print(const string &t_msg)
{
    print(FgColor::cyan, t_msg);
}

/// ***
/// Write a warning message to standard error
/// ***
void scan::StdUtil::warn(const string &t_msg)
{
    print(FgColor::yellow, t_msg);
}

/// ***
/// Enable virtual terminal sequence processing
/// ***
int scan::StdUtil::enable_vt()
{
    if (vt_enabled)
    {
        return 0;
    }
    HANDLE hstdout{ GetStdHandle(STD_OUTPUT_HANDLE) };

    // Invalid stdout handle
    if (hstdout == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }
    ulong stdout_mode{ 0 };

    // Failed to get stdout mode
    if (GetConsoleMode(hstdout, &stdout_mode) == 0)
    {
        return GetLastError();
    }
    stdout_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    // Failed to set stdout mode
    if (SetConsoleMode(hstdout, stdout_mode) == 0)
    {
        return GetLastError();
    }

    vt_enabled = true;
    return 0;
}

/// ***
/// Print message and determine output stream based on color
/// ***
void scan::StdUtil::print(const FgColor &t_fg, const string &t_msg)
{
    // Determine standard stream for output
    std::ostream &os{ (t_fg == FgColor::cyan) ? std::cout : std::cerr };

    // Virtual terminal sequences disabled
    if (!vt_enabled)
    {
        os << m_icons.at(t_fg) << " " << t_msg << LF;
        return;
    }

    // Write color sequence to output stream
    switch (t_fg)
    {
        case FgColor::cyan:    // Cyan FG
            os << CYAN;
            break;
        case FgColor::red:     // Red FG
            os << RED;
            break;
        case FgColor::yellow:  // Yellow FG
            os << YELLOW;
            break;
    }
    os << m_icons.at(t_fg) << RESET << " " << t_msg << LF;
}
