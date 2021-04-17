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

scan::AutoProp<bool> scan::Util::vt_enabled(false);

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
    error(t_ex.msg.get());
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
/// Enable virtual terminal sequence processing
/// ***
const int scan::Util::enable_vt()
{
    if (vt_enabled.get())
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
const size_t scan::Util::count(const string &t_str, const char &t_ch)
{
    if (t_ch == NULL)
    {
        throw NullArgEx("t_ch");
    }
    return static_cast<size_t>(std::count(t_str.begin(), t_str.end(), t_ch));
}

/// ***
/// Convert a long long into a char pointer
/// ***
const char *scan::Util::itoc(const llong &t_num)
{
    if (t_num == NULL)
    {
        throw NullArgEx({ std::to_string(t_num) });
    }

    const string num_s{ itos(t_num) };
    return num_s.empty() ? nullptr : num_s.c_str();
}

/// ***
/// Split a delimited string and return as a vector
/// ***
const scan::Util::vector_s scan::Util::split(const string &t_data,
                                             const string &t_delim) {
    // Default max split to data size
    return split(t_data, t_delim, t_data.size());
}

/// ***
/// Split a delimited string until split limit is reached
/// ***
const scan::Util::vector_s scan::Util::split(const string &t_data,
                                             const string &t_delim,
                                             const size_t &t_max_split) {
    if (t_max_split == NULL)
    {
        throw NullArgEx("t_max_split");
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
/// Indent the given string at each line break
/// ***
const std::string scan::Util::indent(const uint &t_size,
                                     const string &t_data,
                                     const bool t_skip_first) {
    if (t_size == NULL)
    {
        throw NullArgEx("t_size");
    }

    const string tab_buff(t_size, ' ');
    const string eol{ (t_data.find(CRLF) > 0) ? CRLF : LF };

    const vector_s vect{ split(t_data, eol) };
    const size_t length{ vect.size() };

    std::stringstream ss;

    // Indent and insert data into stream
    for (size_t i{ 0 }; i < length; i++)
    {
        // Don't indent first line
        if (t_skip_first && (i == 0))
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
const std::string scan::Util::itos(const llong &t_num)
{
    if (t_num == NULL)
    {
        throw NullArgEx("t_num");
    }
    return std::to_string(t_num);
}

/// ***
/// Strip all instance of a character from a string
/// ***
const std::string scan::Util::strip(const string &t_data, const char &t_ch,
                                                          const bool &t_space) {
    if (t_ch == static_cast<char>(NULL))
    {
        throw NullArgEx("t_ch");
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
const std::string scan::Util::to_lower(const string &t_data)
{
    string clone{ t_data };
    std::transform(clone.begin(), clone.end(), clone.begin(), std::tolower);

    return clone;
}

/// ***
/// Transform UTF-16 encoded string to UTF-8 encoding
/// ***
const std::string scan::Util::utf8(const wstring &t_wdata)
{
    if (t_wdata.empty())
    {
        return string();
    }
    const int len_w{ static_cast<int>(t_wdata.size()) };

    // Calculate required length
    const int len
    {
        WideCharToMultiByte(CP_UTF8, 0, &t_wdata[0], len_w, nullptr, 0, nullptr,
                                                                        nullptr)
    };

    string data(len, 0);
    char *data_ptr{ &data[0] };

    // Populate UTF-8 string
    WideCharToMultiByte(CP_UTF8, 0, t_wdata.c_str(), len_w, data_ptr, len,
                                                                      nullptr,
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
    if (!vt_enabled.get())
    {
        os << m_icons.at(t_fg) << " " << t_msg << LF;
        return;
    }

    // Write color sequence to output stream
    switch (t_fg)
    {
        case FgColor::cyan:
            os << CYAN;
            break;
        case FgColor::red:
            os << RED;
            break;
        case FgColor::yellow:
            os << YELLOW;
            break;
    }
    os << m_icons.at(t_fg) << RESET << " " << t_msg << LF;
}

/// ***
/// Transform UTF-8 encoded string to UTF-16 encoding
/// ***
const std::wstring scan::Util::utf16(const string &t_data)
{
    if (t_data.empty())
    {
        return wstring();
    }

    const char *data_ptr{ &t_data[0] };
    const int len{ static_cast<int>(t_data.size()) };

    // Calculate required length
    int len_w{ MultiByteToWideChar(CP_UTF8, 0, data_ptr, len, nullptr, 0) };
    wstring wdata(len_w, 0);

    // Populate UTF-16 string
    MultiByteToWideChar(CP_UTF8, 0, data_ptr, len, &wdata[0], len_w);
    return wdata;
}
