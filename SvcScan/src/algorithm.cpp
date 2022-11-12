/*
*  algorithm.cpp
*  -------------
*  Source file for range algorithms and utilities
*/
#include <boost/range/algorithm.hpp>
#include <boost/locale/encoding_utf.hpp>
#include "includes/errors/null_arg_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/utils/algorithm.h"

/**
* @brief  Format string decimal point precision.
*/
std::streamsize scan::Algorithm::fstr_precision{ 4 };

/**
* @brief  Determine whether the given data contains only integral numbers.
*/
bool scan::Algorithm::is_integral(const string &t_data)
{
    return ranges::all_of(t_data, [](const char &l_ch) -> bool
    {
        return std::isdigit(l_ch);
    });
}

/**
* @brief  Find the location of the nth substring occurrence in the given data.
*/
std::string::const_iterator scan::Algorithm::find_nth(const string &t_data,
                                                      const string &t_sub,
                                                      const size_t &t_n,
                                                      const bool &t_after) {
    ptrdiff_t offset{ 0 };
    str_iterator iter{ t_data.end() };

    const Range auto range{ boost::find_nth(t_data, t_sub, static_cast<int>(t_n)) };

    if (!range.empty())
    {
        iter = t_after ? range.begin() + t_sub.size() : range.begin();
    }
    return iter;
}

/**
* @brief  Find the location of the nth substring occurrence in the given data.
*/
size_t scan::Algorithm::find_nth_pos(const string &t_data,
                                     const string &t_sub,
                                     const size_t &t_n,
                                     const bool &t_after) {
    size_t offset{ 0 };
    const str_iterator iter{ find_nth(t_data, t_sub, t_n, t_after) };

    if (iter != t_data.end())
    {
        offset = distance(t_data, iter);
    }
    return offset;
}

/**
* @brief  Count the number of substring occurrences in the given data.
*/
size_t scan::Algorithm::count(const string &t_data, const string &t_sub) noexcept
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

/**
* @brief  Erase all substring occurrences from the given data.
*/
std::string scan::Algorithm::erase(const string &t_data, const string &t_sub)
{
    return replace(t_data, t_sub, "");
}

/**
* @brief  Replace all substring occurrences in the given data with a new substring.
*/
std::string scan::Algorithm::replace(const string &t_data,
                                     const string &t_old_sub,
                                     const string &t_new_sub) {

    return boost::replace_all_copy(t_data, t_old_sub, t_new_sub);
}

/**
* @brief  Transform the given 'wchar_t' string into a 'char' string.
*/
std::string scan::Algorithm::str(const wstring &t_wdata)
{
    return boost::locale::conv::utf_to_utf<char>(t_wdata);
}

/**
* @brief  Extract a substring from the given string using the specified iterators.
*/
std::string scan::Algorithm::substr(const string &t_data,
                                    const str_iterator &t_beg_it,
                                    const str_iterator &t_end_it) {

    return t_data.substr(distance(t_data, t_beg_it), distance(t_beg_it, t_end_it));
}

/**
* @brief  Transform the given string characters into their lowercase equivalents.
*/
std::string scan::Algorithm::to_lower(const string &t_data)
{
    return boost::to_lower_copy(t_data);
}

/**
* @brief  Transform the given string characters into their uppercase equivalents.
*/
std::string scan::Algorithm::to_upper(const string &t_data)
{
    return boost::to_upper_copy(t_data);
}

/**
* @brief  Remove all leading and trailing whitespace characters from the given data.
*/
std::string scan::Algorithm::trim(const string &t_data)
{
    return boost::trim_copy(t_data);
}

/**
* @brief  Remove all leading whitespace characters from the given data.
*/
std::string scan::Algorithm::trim_left(const string &t_data)
{
    return boost::trim_left_copy(t_data);
}

/**
* @brief  Remove all trailing whitespace characters from the given data.
*/
std::string scan::Algorithm::trim_right(const string &t_data)
{
    return boost::trim_right_copy(t_data);
}

/**
* @brief  Add an underline to the given data
*/
std::string scan::Algorithm::underline(const string &t_data)
{
    sstream stream;
    stream << t_data << &LF[0] << underline(t_data.size());

    return stream.str();
}

/**
* @brief  Create an underline whose size is equal to the given string size.
*/
std::string scan::Algorithm::underline(const size_t &t_size)
{
    return string(t_size, '-');
}

/**
* @brief  Read the given string data until the first EOL sequence is detected.
*/
std::string scan::Algorithm::upto_first_eol(const string &t_data)
{
    string buffer{ t_data };

    if (!t_data.empty())
    {
        size_t idx{ t_data.find(&CRLF[0]) };

        if (idx != string::npos)
        {
            buffer = t_data.substr(0, idx);
        }
        else if ((idx = t_data.find(&LF[0])) != string::npos)
        {
            buffer = t_data.substr(0, idx);
        }
    }
    return buffer;
}

/**
* @brief  Read the given string data until the last EOL sequence is detected.
*/
std::string scan::Algorithm::upto_last_eol(const string &t_data)
{
    string buffer{ t_data };

    if (!t_data.empty())
    {
        size_t idx{ t_data.rfind(&CRLF[0]) };

        if (idx != string::npos)
        {
            buffer = t_data.substr(0, idx);
        }
        else if ((idx = t_data.rfind(&LF[0])) != string::npos)
        {
            buffer = t_data.substr(0, idx);
        }
    }
    return buffer;
}

/**
* @brief  Transform the given 'char' string into a 'wchar_t' string.
*/
std::wstring scan::Algorithm::wstr(const string &t_data)
{
    return boost::locale::conv::utf_to_utf<wchar_t>(t_data);
}

/**
* @brief  Split the given data using the specified delimiter.
*/
std::vector<std::string> scan::Algorithm::split(const string &t_data,
                                                const string &t_delim) {
    return split(t_data, t_delim, string::npos);
}

/**
* @brief  Split the given data using the specified delimiter
*         until the split limit has been reached.
*/
scan::string_vector scan::Algorithm::split(const string &t_data,
                                           const string &t_delim,
                                           const size_t &t_max_split) {
    if (t_max_split == 0)
    {
        throw NullArgEx{ "t_max_split" };
    }
    string_vector vect;

    if (!t_data.empty())
    {
        if (!t_delim.empty() && t_data.find(t_delim) != string::npos)
        {
            size_t count{ 0 };
            size_t offset{ 0 };

            size_t i{ t_data.find_first_not_of(t_delim, offset) };

            for (i; i != string::npos; i = t_data.find_first_not_of(t_delim, offset))
            {
                if (count++ == t_max_split)
                {
                    vect.push_back(t_data.substr(i));
                    break;
                }

                offset = t_data.find(t_delim, i);
                vect.push_back(t_data.substr(i, offset - i));
            }
        }
        else  // Add the string as a single element
        {
            vect.push_back(t_data);
        }
    }
    return vect;
}
