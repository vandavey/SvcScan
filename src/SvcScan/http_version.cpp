/*
* @file
*     http_version.cpp
* @brief
*     Source file for an HTTP protocol version.
*/
#include "includes/inet/http/http_version.h"

/**
* @brief
*     Initialize the object.
*/
scan::HttpVersion::HttpVersion(const string& t_httpv_str) : HttpVersion{}
{
    string httpv_str{t_httpv_str};
    const string full_prefix{algo::concat(PREFIX, DELIM)};

    // Remove prefix string
    if (httpv_str.starts_with(full_prefix))
    {
        httpv_str = algo::erase(httpv_str, full_prefix);
    }
    const string_array<2> version_nums{algo::split<2>(httpv_str, ".")};

    major = algo::to_uint(version_nums[0]);
    minor = algo::to_uint(version_nums[1]);
}
