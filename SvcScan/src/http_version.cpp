/*
*  http_version.cpp
*  ----------------
*  Source file for an HTTP protocol version
*/
#include "includes/inet/http/http_version.h"
#include "includes/utils/util.h"

/// ***
/// Initialize the object
/// ***
scan::HttpVersion::HttpVersion() noexcept
{
    major = minor = 1U;
}

/// ***
/// Initialize the object
/// ***
scan::HttpVersion::HttpVersion(const HttpVersion &t_httpv)
{
    major = t_httpv.major;
    minor = t_httpv.minor;
}

/// ***
/// Initialize the object
/// ***
scan::HttpVersion::HttpVersion(const uint &t_major, const uint &t_minor)
{
    major = t_major;
    minor = t_minor;
}

/// ***
/// Initialize the object
/// ***
scan::HttpVersion::HttpVersion(const string &t_version_str) : HttpVersion()
{
    string version{ t_version_str };
    const string full_prefix{ Util::fstr("%%", PREFIX, DELIM) };

    // Remove prefix string
    if (Util::starts_with(version, full_prefix))
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

/// ***
/// Cast operator overload
/// ***
scan::HttpVersion::operator uint() const noexcept
{
    return num();
}

/// ***
/// Cast operator overload
/// ***
scan::HttpVersion::operator int() const noexcept
{
    return num();
}

/// ***
/// Cast operator overload
/// ***
scan::HttpVersion::operator string() const
{
    return str();
}

/// ***
/// Get the current version information as an unsigned integer
/// ***
unsigned int scan::HttpVersion::num() const noexcept
{
    return (major * 10U) + minor;
}

/// ***
/// Get the current version number as a string
/// ***
std::string scan::HttpVersion::num_str() const
{
    return Util::fstr("%.%", major, minor);
}

/// ***
/// Get the current version information as a string
/// ***
std::string scan::HttpVersion::str() const
{
    return Util::fstr("%%%", PREFIX, DELIM, num_str());
}
