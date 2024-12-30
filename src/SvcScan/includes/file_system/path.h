/*
* @file
*     path.h
* @brief
*     Header file for file system and path utilities.
*/
#pragma once

#ifndef SCAN_PATH_H
#define SCAN_PATH_H

#include <string>
#include "../console/util.h"
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "file_system_aliases.h"
#include "path_info.h"

/**
* @brief
*     File path and file system utilities.
*/
namespace scan::path
{
    /**
    * @brief
    *     File path and file system constant fields.
    */
    inline namespace defs
    {
        /// @brief  User home file path alias.
        constexpr c_string_t HOME_ALIAS = "~";

        /// @brief  Standard file path delimiter.
        constexpr c_string_t PATH_DELIM = "/";

        /// @brief  Alternative file path delimiter.
        constexpr c_string_t PATH_DELIM_ALT = "\\";

        /// @brief  User profile environment variable name.
        constexpr c_string_t USER_PROFILE = "UserProfile";
    }

    /// @brief  User home directory path.
    inline const string user_home_path{util::env_variable(USER_PROFILE)};

    /**
    * @brief
    *     Normalize the element separators and formatting of the given file path.
    */
    constexpr string& normalize(string& t_path)
    {
        if (!t_path.empty())
        {
            algo::replace(t_path, PATH_DELIM_ALT, PATH_DELIM);

            if (t_path.ends_with(PATH_DELIM))
            {
                t_path.pop_back();
            }
        }
        return t_path;
    }

    /**
    * @brief
    *     Normalize the element separators and formatting of the given file path.
    */
    constexpr string normalize(const string& t_path)
    {
        string buffer{t_path};
        return normalize(buffer);
    }

    bool exists(const string& t_path);
    bool file_or_parent_exists(const string& t_path);
    bool path_exists(const file_path_t& t_file_path);

    PathInfo path_info(const string& t_path);
    PathInfo path_info_not_found(const string& t_path);

    string parent(const string& t_path);
    string& replace_home_alias(string& t_path);
    string replace_home_alias(const string& t_path);
    string& resolve(string& t_path);
    string resolve(const string& t_path);

    file_path_t& replace_home_alias_path(file_path_t& t_file_path);
    file_path_t replace_home_alias_path(const file_path_t& t_file_path);
    file_path_t& resolve_path(file_path_t& t_file_path);
    file_path_t resolve_path(const file_path_t& t_file_path);
}

#endif // !SCAN_PATH_H
