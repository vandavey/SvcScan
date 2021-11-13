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
#include <regex>
#include <windows.h>
#include "includes/except/nullargex.h"
#include "includes/utils/util.h"

/// ***
/// Determine if the given string data starts with the specified substring
/// ***
bool scan::Util::starts_with(const string &t_data, const string &t_sub_str)
{
    return t_data.find(t_sub_str.c_str(), 0) == 0;
}

/// ***
/// Count the number of char occurrences in the given string
/// ***
size_t scan::Util::count(const string &t_data, const char &t_ch)
{
    if (t_ch == NULL)
    {
        throw NullArgEx{ "t_ch" };
    }
    return static_cast<size_t>(std::count(t_data.begin(), t_data.end(), t_ch));
}

/// ***
/// Remove all leading whitespace characters from the given string data
/// ***
std::string scan::Util::lstrip(const string &t_data)
{
    string sbuffer;
    const size_t beg_pos{ t_data.find_first_not_of(' ') };

    if (beg_pos != string::npos)
    {
        sbuffer = t_data.substr(beg_pos);
    }
    return sbuffer;
}

/// ***
/// Replace all substring occurrences with a new substring
/// ***
std::string scan::Util::replace(const string &t_data,
                                const string &t_old_sub,
                                const string &t_new_sub) {

    return std::regex_replace(t_data, std::regex(t_old_sub), t_new_sub);
}

/// ***
/// Remove all trailing whitespace characters from the given string data
/// ***
std::string scan::Util::rstrip(const string &t_data)
{
    string sbuffer;
    const size_t end_pos{ t_data.find_last_not_of(' ') };

    if (end_pos != string::npos)
    {
        sbuffer = t_data.substr(0, end_pos + 1);
    }
    return sbuffer;
}

/// ***
/// Transform wchar_t string into a char string
/// ***
std::string scan::Util::str(const wstring &t_wdata)
{
    string data;

    if (!t_wdata.empty())
    {
        const int wlen{ static_cast<int>(t_wdata.size()) };

        // Calculate required length
        const int len = WideCharToMultiByte(CP_UTF8,
                                            0,
                                            &t_wdata[0],
                                            wlen,
                                            nullptr,
                                            0,
                                            nullptr,
                                            nullptr);
        data = string(len, NULL);

        // Populate char string
        WideCharToMultiByte(CP_UTF8,
                            0,
                            t_wdata.c_str(),
                            wlen,
                            &data[0],
                            len,
                            nullptr,
                            nullptr);
    }
    return data;
}

/// ***
/// Remove all whitespace characters from the given string data
/// ***
std::string scan::Util::strip(const string &t_data)
{
    return replace(t_data, " ", "");
}

/// ***
/// Transform the given string characters to their lowercase equivalent
/// ***
std::string scan::Util::to_lower(const string &t_data)
{
    string sbuffer{ t_data };
    std::transform(sbuffer.begin(), sbuffer.end(), sbuffer.begin(), ::tolower);

    return sbuffer;
}

/// ***
/// Transform the given string characters to their uppercase equivalent
/// ***
std::string scan::Util::to_upper(const string &t_data)
{
    string sbuffer{ t_data };
    std::transform(sbuffer.begin(), sbuffer.end(), sbuffer.begin(), ::toupper);

    return sbuffer;
}

/// ***
/// Transform the given char string into a wchar_t string
/// ***
std::wstring scan::Util::wstr(const string &t_data)
{
    wstring wdata;

    if (!t_data.empty())
    {
        const int len{ static_cast<int>(t_data.size()) };

        // Calculate required length
        int len_w{ MultiByteToWideChar(CP_UTF8, 0, &t_data[0], len, nullptr, 0) };
        wdata = wstring(len_w, NULL);

        // Populate wchar_t string
        MultiByteToWideChar(CP_UTF8, 0, &t_data[0], len, &wdata[0], len_w);
    }
    return wdata;
}

/// ***
/// Split a delimited string and return a new vector
/// ***
scan::Util::vector_s scan::Util::split(const string &t_data, const string &t_delim)
{
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

    // Return vector containing single element
    if (t_delim.empty() || (t_data.find(t_delim) == string::npos))
    {
        vect.push_back(t_data);
        return vect;
    }

    size_t count{ 0 };
    size_t next{ 0 };

    size_t i;

    // Iterate until next separator not found
    while ((i = t_data.find_first_not_of(t_delim, next)) != string::npos)
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
