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
#include "includes/utils/util.h"

/// ***
/// Console foreground color enumeration type
/// ***
enum class scan::Util::FgColor : short
{
    cyan,   // Cyan foreground
    red,    // Red foreground
    yellow  // Yellow foreground
};

scan::AutoProp<bool> scan::Util::vt_enabled{ false };

scan::Util::map_cs scan::Util::m_icons
{
    { FgColor::cyan,   "[*]" },
    { FgColor::yellow, "[!]" },
    { FgColor::red,    "[x]" }
};

/// ***
/// Write an error message to standard error
/// ***
void scan::Util::error(const string &t_msg)
{
    print(FgColor::red, t_msg);
}

/// ***
/// Write formatted error message to standard error
/// ***
void scan::Util::errorf(const string &t_msg, const string &t_arg)
{
    if (t_msg.find('%') == -1)
    {
        throw ArgEx("t_msg", "Missing format character");
    }
    print(FgColor::red, fmt(t_msg, t_arg));
}

/// ***
/// Write exception information to standard error
/// ***
void scan::Util::except(const ArgEx &t_ex)
{
    error(static_cast<string>(t_ex));
    std::cerr << t_ex << LF;
}

/// ***
/// Write exception information to standard error
/// ***
void scan::Util::except(const LogicEx &t_ex)
{
    error(static_cast<string>(t_ex));
    std::cerr << t_ex << LF;
}

/// ***
/// Write general information to standard output
/// ***
void scan::Util::print(const string &t_msg)
{
    print(FgColor::cyan, t_msg);
}

/// ***
/// Write general information to standard output
/// ***
void scan::Util::printf(const string &t_msg, const string &t_arg)
{
    if (t_msg.find('%') == -1)
    {
        throw ArgEx("t_msg", "Missing format character");
    }
    print(FgColor::cyan, fmt(t_msg, t_arg));
}

/// ***
/// Write a warning message to standard error
/// ***
void scan::Util::warn(const string &t_msg)
{
    print(FgColor::yellow, t_msg);
}

/// ***
/// Utility - determine if the given path ends with the substring
/// ***
bool scan::Util::ends_with(const string &t_spath, const string &t_sub)
{
    if (t_spath.empty())
    {
        return false;
    }
    return t_spath.rfind(t_sub) == (t_spath.size() - 1);
}

/// ***
/// Utility - determine if the given path ends with one or more substrings
/// ***
bool scan::Util::ends_with(const string &t_spath, const vector_s &t_svect)
{
    if (t_spath.empty())
    {
        return false;
    }

    // Check each substring for match
    for (const string &sub : t_svect)
    {
        if (ends_with(t_spath, sub))
        {
            return true;
        }
    }
    return false;
}

/// ***
/// Enable virtual terminal sequence processing
/// ***
int scan::Util::enable_vt()
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
/// Count the number of char occurrences in a string
/// ***
size_t scan::Util::count(const string &t_str, const char &t_ch)
{
    if (t_ch == NULL)
    {
        throw NullArgEx{ "t_ch" };
    }
    return static_cast<size_t>(std::count(t_str.begin(), t_str.end(), t_ch));
}

/// ***
/// Split a delimited string and return as a vector
/// ***
scan::Util::vector_s scan::Util::split(const string &t_data,
                                       const string &t_delim) {
    // Default max split to data size
    return split(t_data, t_delim, t_data.size());
}

/// ***
/// Split a delimited string until split limit is reached
/// ***
scan::Util::vector_s scan::Util::split(const string &t_data,
                                       const string &t_delim,
                                       const size_t &t_max_split) {
    if (t_max_split == NULL)
    {
        throw NullArgEx{ "t_max_split" };
    }
    vector_s vect;

    // Return empty string vector
    if (t_data.empty())
    {
        return vect;
    }

    // No delimiter to split on
    if (t_delim.empty())
    {
        vect.push_back(t_data);
        return vect;
    }

    size_t count{ 0 };
    size_t next{ 0 };

    size_t i;

    // Iterate until next separator not found
    while ((i = t_data.find_first_not_of(t_delim, next)) != -1)
    {
        // Add remaining data as element
        if (count == t_max_split)
        {
            vect.push_back(t_data.substr(i, (t_data.size() - 1)));
            break;
        }
        count += 1;

        next = t_data.find(t_delim, i);
        vect.push_back(t_data.substr(i, (next - i)));
    }
    return vect;
}

/// ***
/// Convert a long long into a string
/// ***
std::string scan::Util::itos(const llong &t_num)
{
    if (t_num == NULL)
    {
        throw NullArgEx{ "t_num" };
    }
    return std::to_string(t_num);
}

/// ***
/// Strip all instance of a character from a string
/// ***
std::string scan::Util::strip(const string &t_data, const char &t_ch,
                                                    const bool &t_space) {
    if (t_ch == static_cast<char>(NULL))
    {
        throw NullArgEx{ "t_ch" };
    }
    std::stringstream ss;

    for (const char &ch : t_data)
    {
        // Insert all other chars into stream
        if (ch != t_ch)
        {
            ss << ch;
            continue;
        }

        // Replace <match_ch> with SPACE char
        if (t_space)
        {
            ss << ' ';
        }
    }
    return ss.str();
}

/// ***
/// Transform string characters to their lowercase equivalent
/// ***
std::string scan::Util::to_lower(const string &t_data)
{
    string clone{ t_data };
    std::transform(clone.begin(), clone.end(), clone.begin(), std::tolower);

    return clone;
}

/// ***
/// Transform wchar_t string into a char string
/// ***
std::string scan::Util::str(const wstring &t_wdata)
{
    if (t_wdata.empty())
    {
        return string();
    }
    const int wlen{ static_cast<int>(t_wdata.size()) };

    // Calculate required length
    const int len
    {
        WideCharToMultiByte(CP_UTF8, 0, &t_wdata[0], wlen, nullptr, 0, nullptr,
                                                                       nullptr)
    };

    string data(len, 0);
    char *datap{ &data[0] };

    // Populate char string
    WideCharToMultiByte(CP_UTF8, 0, t_wdata.c_str(), wlen, datap, len, nullptr,
                                                                       nullptr);
    return data;
}

/// ***
/// Print message and determine output stream based on color
/// ***
void scan::Util::print(const FgColor &t_fg, const string &t_msg)
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

/// ***
/// Transform char string into a wchar_t string
/// ***
std::wstring scan::Util::wstr(const string &t_data)
{
    if (t_data.empty())
    {
        return wstring();
    }

    const char *datap{ &t_data[0] };
    const int len{ static_cast<int>(t_data.size()) };

    // Calculate required length
    int len_w{ MultiByteToWideChar(CP_UTF8, 0, datap, len, nullptr, 0) };
    wstring wdata(len_w, 0);

    // Populate wchar_t string
    MultiByteToWideChar(CP_UTF8, 0, datap, len, &wdata[0], len_w);
    return wdata;
}
