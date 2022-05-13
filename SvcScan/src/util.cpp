/*
*  util.cpp
*  --------
*  Source file for algorithms and data type utilities
*/
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <boost/algorithm/string.hpp>
#include "includes/except/null_arg_ex.h"
#include "includes/utils/util.h"

/// ***
/// Determine whether the given data ends with the substring
/// ***
bool scan::Util::ends_with(const string &t_data, const string &t_sub_str)
{
    return boost::ends_with(t_data, t_sub_str);
}

/// ***
/// Determine if the given data ends with one or more substrings
/// ***
bool scan::Util::ends_with(const string &t_data, const vector<string> &t_sub_vect)
{
    bool sep_terminated{ false };

    // Match any of the given terminators
    if (!t_data.empty())
    {
        for (const string &terminator : t_sub_vect)
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
    return ranges::all_of(t_data, [](const char &l_ch) -> bool
    {
        return std::isdigit(l_ch);
    });
}

/// ***
/// Determine whether the given string data starts with the specified substring
/// ***
bool scan::Util::starts_with(const string &t_data, const string &t_sub_str)
{
    return boost::starts_with(t_data, t_sub_str);
}

/// ***
/// Find the location of the nth substring occurrence in the given string
/// ***
std::string::const_iterator scan::Util::find_nth(const string &t_data,
                                                 const string &t_sub,
                                                 const size_t &t_n,
                                                 const bool &t_after) {
    ptrdiff_t offset{ 0 };
    str_iterator iter{ t_data.cend() };

    const iter_range<str_iterator> range = boost::find_nth(t_data,
                                                           t_sub,
                                                           static_cast<int>(t_n));
    if (!range.empty())
    {
        iter = t_after ? (range.begin() + t_sub.size()) : range.begin();
    }
    return iter;
}

/// ***
/// Find the position of the nth substring occurrence in the given string
/// ***
size_t scan::Util::find_nth_pos(const string &t_data,
                                const string &t_sub,
                                const size_t &t_n,
                                const bool &t_after) {

    size_t offset{ 0 };
    const str_iterator iter{ find_nth(t_data, t_sub, t_n, t_after) };

    if (iter != t_data.cend())
    {
        offset = distance(t_data, iter);
    }
    return offset;
}

/// ***
/// Count the number of char occurrences in the given string
/// ***
size_t scan::Util::count(const string &t_data, const string &t_sub) noexcept
{
    size_t count{ 0 };
    size_t offset{ 0 };

    size_t i;

    while ((i = t_data.find_first_of(t_sub, offset)) != string::npos)
    {
        offset = t_data.find(t_sub, i + t_sub.size());
        count++;
    }
    return count;
}

/// ***
/// Remove all leading whitespace characters from the given string data
/// ***
std::string scan::Util::lstrip(const string &t_data)
{
    return boost::trim_left_copy(t_data);
}

/// ***
/// Replace all substring occurrences with a new substring
/// ***
std::string scan::Util::replace(const string &t_data,
                                const string &t_old_sub,
                                const string &t_new_sub) {

    return boost::replace_all_copy(t_data, t_old_sub, t_new_sub);
}

/// ***
/// Replace all substring occurrences with a new substring
/// ***
std::string scan::Util::replace(const string &t_data,
                                const vector<string> &t_old_subs,
                                const string &t_new_sub) {
    string new_data{ t_data };

    // Replace all old substrings
    for (const string &old_sub : t_old_subs)
    {
        boost::replace_all(new_data, old_sub, t_new_sub);
    }
    return new_data;
}

/// ***
/// Remove all trailing whitespace characters from the given string data
/// ***
std::string scan::Util::rstrip(const string &t_data)
{
    return boost::trim_right_copy(t_data);
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
    return boost::trim_copy(t_data);
}

/// ***
/// Extract a substring from the given string using the specified iterators
/// ***
std::string scan::Util::substr(const string &t_data,
                               const str_iterator &t_beg_it,
                               const str_iterator &t_end_it) {

    return t_data.substr(distance(t_data, t_beg_it), distance(t_beg_it, t_end_it));
}

/// ***
/// Transform the given string characters to their lowercase equivalent
/// ***
std::string scan::Util::to_lower(const string &t_data)
{
    return boost::to_lower_copy(t_data);
}

/// ***
/// Transform the given string characters to their uppercase equivalent
/// ***
std::string scan::Util::to_upper(const string &t_data)
{
    return boost::to_upper_copy(t_data);
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
std::vector<std::string> scan::Util::split(const string &t_data,
                                           const string &t_delim) {

    return split(t_data, t_delim, t_data.size());
}

/// ***
/// Split a delimited string until split limit is reached
/// ***
std::vector<std::string> scan::Util::split(const string &t_data,
                                           const string &t_delim,
                                           const size_t &t_max_split) {
    if (t_max_split == NULL)
    {
        throw NullArgEx{ "t_max_split" };
    }
    vector<string> vect;

    // Return empty vector
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
    size_t offset{ 0 };

    size_t i;

    // Iterate until next separator not found
    while ((i = t_data.find_first_not_of(t_delim, offset)) != string::npos)
    {
        // Add remaining data as element
        if (count++ == t_max_split)
        {
            vect.push_back(t_data.substr(i, t_data.size() - 1));
            break;
        }

        offset = t_data.find(t_delim, i);
        vect.push_back(t_data.substr(i, offset - i));
    }
    return vect;
}
