/*
* @file
*     path.cpp
* @brief
*     Source file for file system and path utilities.
*/
#include <cstdlib>
#include <filesystem>
#include "includes/io/filesys/filesys_alias.h"
#include "includes/io/filesys/path.h"
#include "includes/utils/const_defs.h"

/**
* @brief
*     Determine whether the given file path exists.
*/
bool scan::path::exists(const string &t_path)
{
    return t_path.empty() ? false : filesystem::exists(resolve(t_path));
}

/**
* @brief
*     Determine whether the given file path is in its absolute form.
*/
bool scan::path::is_absolute(const string &t_path)
{
    return t_path.empty() ? false : path_t(resolve(t_path)).is_absolute();
}

/**
* @brief
*     Determine whether the given file path leads to a directory.
*/
bool scan::path::is_directory(const string &t_path)
{
    return t_path.empty() ? false : filesystem::is_directory(resolve(t_path));
}

/**
* @brief
*     Determine whether the given file path or its parent exists.
*/
bool scan::path::valid_file(const string &t_path)
{
    const PathInfo info{ path_info(t_path) };
    return info == PathInfo::parent_exists || info == PathInfo::exists;
}

/**
* @brief
*     Get information about the given file path.
*/
scan::PathInfo scan::path::path_info(const string &t_path)
{
    PathInfo info;
    const string full_path{ resolve(t_path) };

    // Determine file path information
    if (full_path.empty())
    {
        info = PathInfo::empty;
    }
    else if (is_directory(full_path))
    {
        info = PathInfo::directory;
    }
    else if (!exists(parent(full_path)))
    {
        info = PathInfo::parent_not_found;
    }
    else if (!exists(full_path))
    {
        info = PathInfo::parent_exists;
    }
    else
    {
        info = PathInfo::exists;
    }

    return info;
}

/**
* @brief
*     Get the parent directory path from the given file path.
*/
std::string scan::path::parent(const string &t_path)
{
    return t_path.empty() ? t_path : path_t(resolve(t_path)).parent_path().string();
}

/**
* @brief
*     Resolve the absolute path of the given relative file path.
*/
std::string scan::path::resolve(const string &t_path)
{
    path_t file_path;

    // Resolve the absolute file path
    if (path_t(t_path).is_absolute())
    {
        file_path = t_path;
    }
    else if (!t_path.empty())
    {
        string_vector path_parts{ parts(t_path) };

        if (path_parts[0] == HOME_ALIAS)
        {
            path_parts[0] = user_home();
        }
        file_path = filesystem::absolute(normalize(algo::join(path_parts, PATH_DELIM)));
    }

    return file_path.string();
}

/**
* @brief
*     Get the absolute home directory file path of the current user.
*/
std::string scan::path::user_home(const string &t_env_var)
{
    string path;

    if (!t_env_var.empty())
    {
        size_t size_required;

        // Calculate required buffer size
        getenv_s(&size_required, nullptr, 0, &t_env_var[0]);

        if (size_required > 0)
        {
            path = string(size_required, CHAR_NULL);
            getenv_s(&size_required, &path[0], size_required, &t_env_var[0]);
        }
    }
    return normalize(&path[0]);
}
