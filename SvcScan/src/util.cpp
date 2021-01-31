/*
*  util.cpp
*  --------
*  Source file for string and standard stream manipulation
*/
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <iostream>
#include <windows.h>
#include "includes/util.h"

namespace Scan
{
    using string = std::string;
    using wstring = std::wstring;
}

Scan::AutoProp<bool> Scan::Util::vt_enabled(false);

/// ***
/// Write an error message to standard error
/// ***
void Scan::Util::error(const string &msg, const bool &newline)
{
    // Virtual terminal sequences disabled
    if (!vt_enabled)
    {
        std::cerr << "[x] " << msg << LF;
        return;
    }
    std::cerr << RED << "[x] " << RESET << msg << LF;

    // Print new line if requested
    if (newline)
    {
        std::cout << LF;
    }
}

/// ***
/// Print a formatted error message to standard error
/// ***
void Scan::Util::errorf(const string &msg, const string &arg,
                                           const bool &newline) {
    if (msg.find('%') == -1)
    {
        throw ArgEx("msg", "Missing format character");
    }
    const string fmsg(fmt(msg, arg));

    // Virtual terminal sequences disabled
    if (!vt_enabled)
    {
        std::cerr << "[x] " << fmsg << LF;
        return;
    }
    std::cerr << RED << "[x] " << RESET << fmsg << LF;

    // Print new line if requested
    if (newline)
    {
        std::cout << LF;
    }
}

/// ***
/// Write exception information to standard error
/// ***
void Scan::Util::except(const ArgEx &ex)
{
    error(ex.msg.get());
    std::cerr << ex << LF;
}

/// ***
/// Write general information to standard output
/// ***
void Scan::Util::print(const string &msg)
{
    // Virtual terminal sequences disabled
    if (!vt_enabled)
    {
        std::cerr << "[*] " << msg << LF;
        return;
    }
    std::cout << CYAN << "[*] " << RESET << msg << LF;
}

/// ***
/// Write a warning message to standard error
/// ***
void Scan::Util::warn(const string &msg)
{
    // Virtual terminal sequences disabled
    if (!vt_enabled)
    {
        std::cerr << "[!] " << msg << LF;
        return;
    }
    std::cerr << YELLOW << "[!] " << RESET << msg << LF;
}

/// ***
/// Write a formatted warning message to standard error
/// ***
void Scan::Util::warnf(const string &msg, const string &arg,
                                          const bool &newline) {
    if (msg.find('%') == -1)
    {
        throw ArgEx("msg", "Unable to locate format char: '%'");
    }
    const string fmsg(fmt(msg, arg));

    // Virtual terminal sequences disabled
    if (!vt_enabled)
    {
        std::cerr << "[!] " << fmsg << LF;
        return;
    }
    std::cerr << YELLOW << "[!] " << RESET << fmsg << LF;

    // Print new line if requested
    if (newline)
    {
        std::cout << LF;
    }
}

/// ***
/// Enable virtual terminal sequence processing
/// ***
const int Scan::Util::enable_vt()
{
    if (vt_enabled.get())
    {
        return 0;
    }
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

    // Failed to set stdout mode
    if (SetConsoleMode(hstdout, stdout_mode) == 0)
    {
        return GetLastError();
    }

    vt_enabled = true;
    return 0;
}

/// ***
/// Count the number of char occurrences in a string
/// ***
const size_t Scan::Util::count(const string &str, const char &ch)
{
    if (ch == NULL)
    {
        throw NullArgEx("ch");
    }
    return static_cast<size_t>(std::count(str.begin(), str.end(), ch));
}

/// ***
/// Convert a long long into a char pointer
/// ***
const char *Scan::Util::itoc(const llong &num)
{
    if (num == NULL)
    {
        throw NullArgEx({std::to_string(num)});
    }

    const string num_s(itos(num));
    return num_s.empty() ? nullptr : num_s.c_str();
}

/// ***
/// Split a delimited string and return as a vector
/// ***
const Scan::Util::vector_s Scan::Util::split(const string &data,
                                             const string &delim) {
    return split(data, delim, data.size());
}

/// ***
/// Split a delimited string until split limit is reached
/// ***
const Scan::Util::vector_s Scan::Util::split(const string &data,
                                             const string &delim,
                                             const size_t &max_split) {
    if (max_split == NULL)
    {
        throw NullArgEx("max_split");
    }
    vector_s vect;

    // Return empty string vector
    if (data.empty())
    {
        return vect;
    }

    // No delimiter to split on
    if (delim.empty())
    {
        vect.push_back(data);
        return vect;
    }

    size_t i;
    size_t count = {0}, next = {0};

    // Iterate until next separator not found
    while ((i = data.find_first_not_of(delim, next)) != -1)
    {
        // Add remaining data as element
        if (count == max_split)
        {
            vect.push_back(data.substr(i, (data.size() - 1)));
            break;
        }
        count += 1;

        next = data.find(delim, i);
        vect.push_back(data.substr(i, (next - i)));
    }
    return vect;
}

/// ***
/// Indent the given string at each line break
/// ***
const std::string Scan::Util::indent(const string &data,
                                     const uint &tab_size,
                                     const bool skip_first) {
    if (tab_size == NULL)
    {
        throw NullArgEx("tab_size");
    }

    const string tab_buff(tab_size, ' ');
    const string eol((data.find(CRLF) > 0) ? CRLF : LF);

    const vector_s vect(split(data, eol));
    const size_t length = {vect.size()};

    std::stringstream ss;

    // Indent and insert data into stream
    for (size_t i = {0}; i < length; i++)
    {
        // Don't indent first line
        if (skip_first && (i == 0))
        {
            ss << vect[i] << LF;
            continue;
        }
        ss << tab_buff << vect[i];

        // Add line break if not last item
        if (i < (length - 1))
        {
            ss << LF;
        }
    }
    return ss.str();
}

/// ***
/// Convert a long long into a string
/// ***
const std::string Scan::Util::itos(const llong &num)
{
    if (num == NULL)
    {
        throw NullArgEx("num");
    }
    return std::to_string(num);
}

/// ***
/// Transform UTF-16 encoded string to UTF-8 encoding
/// ***
const std::string Scan::Util::utf8(const wstring &data_w)
{
    if (data_w.empty())
    {
        return string();
    }
    const int len_w = {static_cast<int>(data_w.size())};

    // Calculate required length
    const int len = WideCharToMultiByte(CP_UTF8, 0, &data_w[0], len_w, NULL,
                                                                0, NULL, NULL);
    string data(len, 0);

    // Populate UTF-8 string
    WideCharToMultiByte(CP_UTF8, 0, data_w.c_str(), len_w, &data[0], len,
                                                           NULL, NULL);
    return data;
}

/// ***
/// Transform UTF-8 encoded string to UTF-16 encoding
/// ***
const std::wstring Scan::Util::utf16(const string &data)
{
    if (data.empty())
    {
        return wstring();
    }
    const int len = {static_cast<int>(data.size())};
    const char *ptr = {&data[0]};

    // Calculate required length
    int len_w = {MultiByteToWideChar(CP_UTF8, 0, ptr, len, NULL, 0)};
    wstring data_w(len_w, 0);

    // Populate UTF-16 string
    MultiByteToWideChar(CP_UTF8, 0, ptr, len, &data_w[0], len_w);
    return data_w;
}
