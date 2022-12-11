/*
*  algorithm.cpp
*  -------------
*  Source file for range algorithms and utilities
*/
#include <boost/range/algorithm.hpp>
#include "includes/errors/null_arg_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/io/std_util.h"
#include "includes/utils/algorithm.h"

/**
* @brief  Format string decimal point precision.
*/
std::streamsize scan::Algorithm::fstr_precision{ 4 };

/**
* @brief  Determine whether the given data contains only integral numbers.
*         Optionally consider only unsigned integral numbers as valid.
*/
bool scan::Algorithm::is_integral(const string &t_data, const bool &t_unsigned)
{
    const bool digit = ranges::all_of(t_data, [](const char &l_ch) -> bool
    {
        return std::isdigit(l_ch);
    });
    return t_unsigned ? digit && std::stoi(t_data) >= 0 : digit;
}

/**
* @brief  Convert the given data to a 16-bit unsigned integer.
*/
scan::word_t scan::Algorithm::to_word(const string &t_data)
{
    return static_cast<word_t>(to_uint(t_data));
}

/**
* @brief  Convert the given data to a 32-bit unsigned integer.
*/
scan::uint_t scan::Algorithm::to_uint(const string &t_data)
{
    if (!is_integral(t_data, true))
    {
        throw ArgEx{ "t_data", "Data must be integral" };
    }
    return static_cast<uint_t>(std::abs(std::stoi(t_data)));
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
* @brief  Erase all occurrences of the specified substring from the given data.
*/
std::string scan::Algorithm::erase(const string &t_data, const string &t_sub)
{
    return replace(t_data, t_sub, "");
}

/**
* @brief  Create a title using the given label and value. Optionally specify
*         the underline character and whether the results should be colorized.
*/
std::string scan::Algorithm::make_title(const string &t_title_lbl,
                                        const string &t_title_val,
                                        const bool &t_colorize,
                                        const char &t_ln_char) {
    string title_lbl{ t_title_lbl };
    const string title_val{ fstr(" : %", t_title_val) };

    const size_t ln_size{ title_lbl.size() + title_val.size() };

    if (t_colorize)
    {
        title_lbl = StdUtil::colorize(title_lbl, Color::green);
    }
    const string title{ concat(title_lbl, title_val) };

    return concat(title, &LF[0], underline(ln_size, t_ln_char), &LF[0]);
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
* @brief  Add an underline to the given data.
*/
std::string scan::Algorithm::underline(const string &t_data, const char &t_ln_char)
{
    if (t_ln_char == CHAR_NULL)
    {
        throw NullArgEx{ "t_ln_char" };
    }
    return concat(t_data, &LF[0], underline(t_data.size(), t_ln_char));
}

/**
* @brief  Colorize and add an underline to the given data.
*/
std::string scan::Algorithm::underline(const string &t_data,
                                       const Color t_color,
                                       const char &t_ln_char) {
    if (t_ln_char == CHAR_NULL)
    {
        throw NullArgEx{ "t_ln_char" };
    }

    const size_t ln_size{ t_data.size() };
    const string colored_data{ StdUtil::colorize(t_data, t_color) };

    return concat(colored_data, &LF[0], underline(ln_size, t_ln_char));
}

/**
* @brief  Create an underline whose size is equal to the given string size.
*/
std::string scan::Algorithm::underline(const size_t &t_size, const char &t_ln_char)
{
    if (t_ln_char == CHAR_NULL)
    {
        throw NullArgEx{ "t_ln_char" };
    }
    return string(t_size, t_ln_char);
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
* @brief  Split the given data using the specified delimiter.
*/
std::vector<std::string> scan::Algorithm::split(const string &t_data,
                                                const string &t_delim) {
    return split(t_data, t_delim, string::npos);
}

/**
* @brief  Split the given data using the specified delimiter into a vector
*         whose size is less than or equal to the specified element count.
*/
scan::string_vector scan::Algorithm::split(const string &t_data,
                                           const string &t_delim,
                                           const size_t &t_count) {
    if (t_count == 0)
    {
        throw NullArgEx{ "t_count" };
    }
    string_vector vect;

    if (!t_delim.empty() && t_data.find(t_delim) != string::npos)
    {
        size_t offset{ 0 };
        size_t split_count{ 0 };

        size_t i{ t_data.find_first_not_of(t_delim, offset) };

        // Split the data and add the results to the vector
        for (i; i != string::npos; i = t_data.find_first_not_of(t_delim, offset))
        {
            if (split_count++ == t_count - 1)
            {
                vect.push_back(t_data.substr(i));
                break;
            }

            offset = t_data.find(t_delim, i);
            vect.push_back(t_data.substr(i, offset - i));
        }
    }
    else if (!t_data.empty())
    {
        vect.push_back(t_data);
    }

    return vect;
}
