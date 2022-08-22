/*
*  http_version.cpp
*  ----------------
*  Source file for an HTTP protocol version
*/
#include "includes/inet/http/http_version.h"
#include "includes/utils/util.h"

/**
* @brief  Initialize the object.
*/
scan::HttpVersion::HttpVersion() noexcept
{
    major = minor = 1U;
}

/**
* @brief  Initialize the object.
*/
scan::HttpVersion::HttpVersion(const HttpVersion &t_httpv)
{
    *this = t_httpv;
}

/**
* @brief  Initialize the object.
*/
scan::HttpVersion::HttpVersion(const uint &t_major, const uint &t_minor)
{
    major = t_major;
    minor = t_minor;
}

/**
* @brief  Initialize the object.
*/
scan::HttpVersion::HttpVersion(const string &t_version_str) : this_t()
{
    string version{ t_version_str };
    const string full_prefix{ Util::fstr("%%", PREFIX, DELIM) };

    // Remove prefix string
    if (version.starts_with(full_prefix))
    {
        version = Util::replace(version, full_prefix, "");
    }
    const vector<string> version_nums{ Util::split(version, ".", 1) };

    if (version_nums.size() == 2)
    {
        major = std::stoi(version_nums[0]);
        minor = std::stoi(version_nums[1]);
    }
}

/**
* @brief  Copy assignment operator overload.
*/
scan::HttpVersion &scan::HttpVersion::operator=(const HttpVersion &t_httpv) noexcept
{
    major = t_httpv.major;
    minor = t_httpv.minor;

    return *this;
}

/**
* @brief  Cast operator overload.
*/
scan::HttpVersion::operator uint() const noexcept
{
    return num();
}

/**
* @brief  Cast operator overload.
*/
scan::HttpVersion::operator int() const noexcept
{
    return num();
}

/**
* @brief  Cast operator overload.
*/
scan::HttpVersion::operator string() const
{
    return str();
}

/**
* @brief  Get the calculated current version number.
*/
unsigned int scan::HttpVersion::num() const noexcept
{
    return (major * 10U) + minor;
}

/**
* @brief  Get the current version number as a string in decimal format.
*/
std::string scan::HttpVersion::num_str() const
{
    return Util::fstr("%.%", major, minor);
}

/**
* @brief  Get the current version number as a string that can be used
*         in the start-line of an HTTP request (e.g., HTTP/1.1).
*/
std::string scan::HttpVersion::str() const
{
    return Util::fstr("%%%", PREFIX, DELIM, num_str());
}
