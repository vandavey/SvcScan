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
#include "includes/except/nullargex.h"
#include "includes/utils/util.h"

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
scan::Util::vector_s scan::Util::split(const string &t_data, const string &t_delim)
{
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
    const int len = { WideCharToMultiByte(CP_UTF8,
                                          0,
                                          &t_wdata[0],
                                          wlen,
                                          nullptr,
                                          0,
                                          nullptr,
                                          nullptr) };
    string data(len, 0);
    char *datap{ &data[0] };

    // Populate char string
    WideCharToMultiByte(CP_UTF8,
                        0,
                        t_wdata.c_str(),
                        wlen,
                        datap,
                        len,
                        nullptr,
                        nullptr);
    return data;
}

/// ***
/// Strip all instance of a character from a string
/// ***
std::string scan::Util::strip(const string &t_data,
                              const char &t_ch,
                              const bool &t_space) {
    if (t_ch == char(NULL))
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
