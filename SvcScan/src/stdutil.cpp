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
    print_status(std::cerr, RED, "[x]", t_msg);
}

/// ***
/// Write exception information to standard error
/// ***
void scan::StdUtil::except(const ArgEx &t_ex)
{
    std::cerr << LF;
    print_color(std::cerr, RED, t_ex);
}

/// ***
/// Write exception information to standard error
/// ***
void scan::StdUtil::except(const LogicEx &t_ex)
{
    std::cerr << LF;
    print_color(std::cerr, RED, t_ex);
}

/// ***
/// Write status information to standard output
/// ***
void scan::StdUtil::info(const string &t_msg)
{
    print_status(std::cout, GREEN, "[+]", t_msg);
}

/// ***
/// Write general information to standard output
/// ***
void scan::StdUtil::print(const string &t_msg)
{
    print_status(std::cout, CYAN, "[*]", t_msg);
}

/// ***
/// Write a warning message to standard error
/// ***
void scan::StdUtil::warn(const string &t_msg)
{
    print_status(std::cerr, YELLOW, "[!]", t_msg);
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
/// Print all contents of the given string in the specified foreground color
/// ***
void scan::StdUtil::print_color(ostream &t_os,
                                const string &t_fg,
                                const string &t_msg) {

    // Write color sequence to output stream
    if (vt_enabled)
    {
        t_os << t_fg << t_msg << RESET << LF;
    }
    else  // Virtual terminal sequences disabled
    {
        t_os << t_msg << LF;
    }
}

// ***
/// Print a customized status to the given output stream
/// ***
void scan::StdUtil::print_status(ostream &t_os,
                                 const string &t_fg,
                                 const string &t_symbol,
                                 const string &t_msg) {

    // Write color sequence/message to output stream
    if (vt_enabled)
    {
        t_os << t_fg << t_symbol << RESET << " " << t_msg << LF;
    }
    else  // Virtual terminal sequences disabled
    {
        t_os << t_symbol << " " << t_msg << LF;
    }
}
