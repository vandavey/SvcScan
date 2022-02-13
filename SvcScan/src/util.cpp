/*
*  util.cpp
*  --------
*  Source file for string and data-type utilities
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
/// Determine whether the given data ends with the substring
/// ***
bool scan::Util::ends_with(const string &t_data, const string &t_sub_str)
{
    return !t_data.empty() && (t_data.rfind(t_sub_str) == (t_data.size() - 1));
}

/// ***
/// Determine if the given data ends with one or more substrings
/// ***
bool scan::Util::ends_with(const string &t_data, const vector_s &t_sub_strs)
{
    bool sep_terminated{ false };

    // Match any of the given terminators
    if (!t_data.empty())
    {
        for (const string &terminator : t_sub_strs)
        {
            if (sep_terminated = ends_with(t_data, terminator))
            {
                break;
            }
        }
    }
    return sep_terminated;
}

/// ***
/// Determine whether the given string data is an integral number
/// ***
bool scan::Util::is_integral(const string &t_data)
{
    return std::all_of(t_data.cbegin(), t_data.cend(), [](const char &l_ch)
    {
        return std::isdigit(l_ch);
    });
}

/// ***
/// Determine whether the given string data starts with the specified substring
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
    sbuffer.shrink_to_fit();

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
/// Replace all substring occurrences with a new substring
/// ***
std::string scan::Util::replace(const string &t_data,
                                const vector_s &t_old_subs,
                                const string &t_new_sub) {
    string new_data{ t_data };

    // Replace all old substrings
    for (const string &old_sub : t_old_subs)
    {
        new_data = replace(new_data, old_sub, t_new_sub);
    }
    return new_data;
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
    sbuffer.shrink_to_fit();

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
                                            NULL,
                                            &t_wdata[0],
                                            wlen,
                                            nullptr,
                                            0,
                                            nullptr,
                                            nullptr);
        data = string(len, NULL);

        // Populate char string
        WideCharToMultiByte(CP_UTF8,
                            NULL,
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
    return Util::lstrip(Util::rstrip(t_data));
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
        int len_w{ MultiByteToWideChar(CP_UTF8, NULL, &t_data[0], len, nullptr, 0) };
        wdata = wstring(len_w, NULL);

        // Populate wchar_t string
        MultiByteToWideChar(CP_UTF8, NULL, &t_data[0], len, &wdata[0], len_w);
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

/// ***
/// Helper method used to variadically interpolate arguments in a string
/// ***
std::string scan::Util::fstr(const string &t_data)
{
    return t_data;
}
