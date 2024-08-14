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
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "../utils/literals.h"
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
        constexpr cstr_t HOME_ALIAS = "~";

        /// @brief  Standard file path delimiter.
        constexpr cstr_t PATH_DELIM = "/";

        /// @brief  Alternative file path delimiter.
        constexpr cstr_t PATH_DELIM_ALT = "\\";

        /// @brief  User profile environment variable name.
        constexpr cstr_t USER_PROFILE = "USERPROFILE";
    }

    /**
    * @brief
    *     Normalize the element separators and formatting of the given file path.
    */
    constexpr string normalize(const string& t_path)
    {
        string path;

        if (!t_path.empty())
        {
            path = algo::replace(t_path, PATH_DELIM_ALT, PATH_DELIM);

            if (path.ends_with(PATH_DELIM))
            {
                path = path.substr(0_st, path.size() - 1_st);
            }
        }
        return path;
    }

    /**
    * @brief
    *     Get a vector containing all of the given file path's elements.
    */
    constexpr string_vector parts(const string& t_path)
    {
        string_vector parts;

        if (!t_path.empty())
        {
            parts = algo::split(normalize(t_path), PATH_DELIM);
        }
        return parts;
    }

    bool exists(const string& t_path);
    bool file_or_parent_exists(const string& t_path);
    bool is_absolute(const string& t_path);

    PathInfo path_info(const string& t_path);

    string parent(const string& t_path);
    string resolve(const string& t_path);
    string user_home();
}

#endif // !SCAN_PATH_H
