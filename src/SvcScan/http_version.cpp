/*
* @file
*     http_version.cpp
* @brief
*     Source file for an HTTP protocol version.
*/
#include "includes/inet/http/http_version.h"
#include "includes/inet/net_expr.h"
#include "includes/utils/algorithm.h"

/**
* @brief
*     Initialize the object.
*/
scan::HttpVersion::HttpVersion() noexcept
{
    major = minor = 1U;
}

/**
* @brief
*     Initialize the object.
*/
scan::HttpVersion::HttpVersion(const HttpVersion &t_httpv) noexcept
{
    *this = t_httpv;
}

/**
* @brief
*     Initialize the object.
*/
scan::HttpVersion::HttpVersion(const uint_t &t_major, const uint_t &t_minor) noexcept
{
    major = t_major;
    minor = t_minor;
}

/**
* @brief
*     Initialize the object.
*/
scan::HttpVersion::HttpVersion(const string &t_httpv_str) : HttpVersion()
{
    string httpv_str{ t_httpv_str };
    const string full_prefix{ algo::concat(PREFIX, DELIM) };

    // Remove prefix string
    if (httpv_str.starts_with(full_prefix))
    {
        httpv_str = algo::erase(httpv_str, full_prefix);
    }
    const string_array<2> version_nums{ algo::split<2>(httpv_str, ".") };

    major = algo::to_uint(version_nums[0]);
    minor = algo::to_uint(version_nums[1]);
}

/**
* @brief
*     Copy assignment operator overload.
*/
scan::HttpVersion &scan::HttpVersion::operator=(const HttpVersion &t_httpv) noexcept
{
    major = t_httpv.major;
    minor = t_httpv.minor;

    return *this;
}

/**
* @brief
*     Cast operator overload.
*/
scan::HttpVersion::operator scan::uint_t() const noexcept
{
    return num();
}

/**
* @brief
*     Cast operator overload.
*/
scan::HttpVersion::operator int() const noexcept
{
    return static_cast<int>(num());
}

/**
* @brief
*     Cast operator overload.
*/
scan::HttpVersion::operator std::string() const
{
    return str();
}

/**
* @brief
*     Get the calculated current version number.
*/
unsigned int scan::HttpVersion::num() const noexcept
{
    return (major * 10U) + minor;
}

/**
* @brief
*     Get the current version number as a string in decimal format.
*/
std::string scan::HttpVersion::num_str() const
{
    return algo::fstr("%.%", major, minor);
}

/**
* @brief
*     Get the current version number as a string that can be used
*     in the start-line of an HTTP request (e.g., HTTP/1.1).
*/
std::string scan::HttpVersion::str() const
{
    return algo::concat(PREFIX, DELIM, num_str());
}
