/*
* @file
*     path.cpp
* @brief
*     Source file for file system and path utilities.
*/
#include <cstdlib>
#include <filesystem>
#include "includes/file_system/file_system_aliases.h"
#include "includes/file_system/path.h"
#include "includes/utils/const_defs.h"

/**
* @brief
*     Determine whether the given file path exists.
*/
bool scan::path::exists(const string& t_path)
{
    return t_path.empty() ? false : filesystem::exists(resolve(t_path));
}

/**
* @brief
*     Determine whether the given file path is in its absolute form.
*/
bool scan::path::is_absolute(const string& t_path)
{
    return t_path.empty() ? false : path_t(t_path).is_absolute();
}

/**
* @brief
*     Determine whether the given file path or its parent directory path exists.
*/
bool scan::path::file_or_parent_exists(const string& t_path)
{
    return algo::any_equal(path_info(t_path), PathInfo::file, PathInfo::new_file);
}

/**
* @brief
*     Get information about the given file path.
*/
scan::PathInfo scan::path::path_info(const string& t_path)
{
    const string full_path{resolve(t_path)};
    PathInfo info{t_path.empty() ? PathInfo::empty : PathInfo::unknown};

    if (!t_path.empty())
    {
        switch (filesystem::status(resolve(t_path)).type())
        {
            case file_type::not_found:
                info = exists(parent(t_path)) ? PathInfo::new_file : PathInfo::not_found;
                break;
            case file_type::directory:
                info = PathInfo::directory;
                break;
            case file_type::regular:
            case file_type::symlink:
                info = PathInfo::file;
                break;
            default:
                break;
        }
    }
    return info;
}

/**
* @brief
*     Get the parent directory path from the given file path.
*/
std::string scan::path::parent(const string& t_path)
{
    return t_path.empty() ? t_path : path_t(resolve(t_path)).parent_path().string();
}

/**
* @brief
*     Resolve the absolute path of the given relative file path.
*/
std::string scan::path::resolve(const string& t_path)
{
    path_t file_path;

    // Resolve the absolute file path
    if (is_absolute(t_path))
    {
        file_path = t_path;
    }
    else if (!t_path.empty())
    {
        string_vector path_parts{parts(t_path)};

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
std::string scan::path::user_home()
{
    string path;
    size_t size_required;

    // Calculate required buffer size
    getenv_s(&size_required, nullptr, 0_st, USER_PROFILE);

    if (size_required > 0)
    {
        path = string(size_required, CHAR_NULL);
        getenv_s(&size_required, &path[0], size_required, USER_PROFILE);
    }
    return normalize(&path[0]);
}
