/*
* @file
*     algorithm.cpp
* @brief
*     Source file for range algorithms and utilities.
*/
#include <cctype>
#include <cmath>
#include <regex>
#include "includes/errors/arg_ex.h"
#include "includes/io/std_util.h"
#include "includes/utils/algorithm.h"

/**
* @brief
*     Determine whether the given data contains only integral numbers.
*     Optionally consider only unsigned integral numbers as valid.
*/
bool scan::algo::is_integral(const string &t_data, const bool &t_unsigned)
{
    const bool digit = ranges::all_of(t_data, [](const char &l_ch) -> bool
    {
        return std::isdigit(l_ch);
    });
    return t_unsigned ? digit && std::stoi(t_data) >= 0 : digit;
}

/**
* @brief
*     Determine whether the given data matches the specified regular expression pattern.
*/
bool scan::algo::matches(const string &t_data, const string &t_rgx_pattern)
{
    return std::regex_match(t_data, std::regex(t_rgx_pattern));
}

/**
* @brief
*     Convert the given data to a 16-bit unsigned integer.
*/
uint16_t scan::algo::to_word(const string &t_data)
{
    return static_cast<uint16_t>(to_uint(t_data));
}

/**
* @brief
*     Convert the given data to an unsigned integer.
*/
scan::uint_t scan::algo::to_uint(const string &t_data)
{
    if (!is_integral(t_data, true))
    {
        throw ArgEx{ "t_data", "Data must be integral" };
    }
    return static_cast<uint_t>(std::abs(std::stoi(t_data)));
}

/**
* @brief
*     Transform the given string characters into their lowercase equivalents.
*/
std::string scan::algo::to_lower(const string &t_data)
{
    string buffer{ t_data };
    ranges::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);
    return buffer;
}

/**
* @brief
*     Transform the given string characters into their uppercase equivalents.
*/
std::string scan::algo::to_upper(const string &t_data)
{
    string buffer{ t_data };
    ranges::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
    return buffer;
}

/**
* @brief
*     Add an underline to the given data.
*/
std::string scan::algo::underline(const string &t_data, const char &t_ln_char)
{
    const char ln_char{ t_ln_char == CHAR_NULL ? CHAR_DASH : t_ln_char };
    return concat(t_data, LF, underline(t_data.size(), ln_char));
}

/**
* @brief
*     Colorize and add an underline to the given data.
*/
std::string scan::algo::underline(const string &t_data,
                                  const Color t_color,
                                  const char &t_ln_char)
{
    const string colored_data{ StdUtil::colorize(t_data, t_color) };
    const char ln_char{ t_ln_char == CHAR_NULL ? CHAR_DASH : t_ln_char };

    return concat(colored_data, LF, underline(t_data.size(), ln_char));
}
