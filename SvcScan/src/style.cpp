#include <iostream>
#include <sstream>
#include "../include/style.h"

#ifdef WIN_OS
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif // !WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif // WIN_OS

namespace Scan
{
    using std::endl;
    using std::string;
    using std::wstring;
}

Scan::AutoProp<bool> Scan::Style::vt_mode = false;

/// ***
/// Write an error message to standard error
/// ***
void Scan::Style::error(const string &msg)
{
    // Virtual terminal processing disabled
    if (!vt_mode)
    {
        std::cerr << "[x] " << msg << endl;
        return;
    }
    std::wcerr << RED << L"[x] " << RESET << utf16(msg) << endl;
}

/// ***
/// Print a formatted error message to standard error
/// ***
void Scan::Style::error(const string &msg, const string &arg)
{
    std::wstringstream ss;
    const string fmsg = {fmt(msg, arg)};

    // Virtual terminal processing disabled
    if (!vt_mode)
    {
        std::cerr << "[x] " << fmsg << endl;
        return;
    }
    ss << RED << L"[x] " << RESET << utf16(fmsg) << endl;
    std::wcerr << ss.str();
}

/// ***
/// Write general information to standard output
/// ***
void Scan::Style::print(const string &msg)
{
    // Virtual terminal processing disabled
    if (!vt_mode)
    {
        std::cerr << "[*] " << msg << endl;
        return;
    }
    std::wcout << CYAN << L"[*] " << RESET << utf16(msg) << endl;
}

/// ***
/// Write a warning message to standard error
/// ***
void Scan::Style::warning(const string &msg)
{
    // Virtual terminal processing disabled
    if (!vt_mode)
    {
        std::cerr << "[!] " << msg << endl;
        return;
    }
    std::wcerr << YELLOW << L"[!] " << RESET << utf16(msg) << endl;
}

/// ***
/// Enable virtual terminal sequence processing (Windows only)
/// ***
const int Scan::Style::enable_vtmode()
{
#ifdef WIN_OS
    HANDLE hstdout = {GetStdHandle(STD_OUTPUT_HANDLE)};

    // Invalid stdout handle
    if (hstdout == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }
    ulong stdout_mode = {0};

    // Failed to get stdout mode
    if (GetConsoleMode(hstdout, &stdout_mode) == 0)
    {
        return GetLastError();
    }
    stdout_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    // Failed to get stdout mode
    if (SetConsoleMode(hstdout, stdout_mode) == 0)
    {
        return GetLastError();
    }

    vt_mode = true;
    return 0;
#else
    return -1;
#endif // WIN_OS
}

/// ***
/// Interpolate string with argument at '%' position(s)
/// ***
template<class T>
const std::string Scan::Style::fmt(const string &msg, const T &arg)
{
    std::stringstream ss;

    if (msg.find('%') == -1)
    {
        std::cerr << "Unable to locate '%' in message" << endl;
        return ss.str();
    }

    // Print newly formatted data
    for (int i = {0}; i < msg.length(); i++)
    {
        (msg[i] == '%') ? (ss << arg) : (ss << msg[i]);
    }
    return ss.str();
}

#ifdef WIN_OS
/// ***
/// Transform UTF-8 encoding to UTF-16 encoding
/// ***
const std::wstring Scan::Style::utf16(const string &data)
{
    if (data.empty())
    {
        return wstring();
    }
    const int len = {(int)data.size()};
    const char *ptr = {&data[0]};

    // Calculate required length
    int len_w = {MultiByteToWideChar(CP_UTF8, 0, ptr, len, NULL, 0)};
    wstring data_w(len_w, 0);

    // Populate UTF-16 string
    MultiByteToWideChar(CP_UTF8, 0, ptr, len, &data_w[0], len_w);
    return data_w;
}
#endif // WIN_OS
