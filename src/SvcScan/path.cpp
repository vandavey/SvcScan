/*
* @file
*     path.cpp
* @brief
*     Source file for file system and path utilities.
*/
#include <filesystem>
#include <system_error>
#include "includes/file_system/path.h"

/**
* @brief
*     Determine whether the given file path exists.
*/
bool scan::path::exists(const string& t_path)
{
    std::error_code ecode;
    return !t_path.empty() && filesystem::exists(resolve(t_path), ecode);
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
*     Determine whether the given standard library file path exists.
*/
bool scan::path::path_exists(const file_path_t& t_file_path)
{
    std::error_code ecode;
    return !t_file_path.empty() && filesystem::exists(resolve_path(t_file_path), ecode);
}

/**
* @brief
*     Get information about the given file path.
*/
scan::PathInfo scan::path::path_info(const string& t_path)
{
    PathInfo info;
    const string full_path{resolve(t_path)};

    switch (filesystem::status(full_path).type())
    {
        case file_type::none:
            info = PathInfo::empty;
            break;
        case file_type::not_found:
            info = path_info_not_found(full_path);
            break;
        case file_type::directory:
            info = PathInfo::directory;
            break;
        case file_type::regular:
        case file_type::symlink:
            info = PathInfo::file;
            break;
        default:
            info = PathInfo::unknown;
            break;
    }
    return info;
}

/**
* @brief
*     Get information about the given file path whose file type is not found.
*/
scan::PathInfo scan::path::path_info_not_found(const string& t_path)
{
    PathInfo info{PathInfo::not_found};
    const file_path_t file_path{resolve(t_path)};

    if (file_path.empty())
    {
        info = PathInfo::empty;
    }
    else if (path_exists(file_path.parent_path()))
    {
        info = file_path.has_extension() ? PathInfo::new_file : PathInfo::new_directory;
    }
    return info;
}

/**
* @brief
*     Get the parent directory path from the given file path.
*/
std::string scan::path::parent(const string& t_path)
{
    return t_path.empty() ? t_path : resolve_path(t_path).parent_path().string();
}

/**
* @brief
*     Replace the home alias (`~`) in the given file path
*     with the resolved user home profile directory path.
*/
std::string& scan::path::replace_home_alias(string& t_path)
{
    if (algo::contains(t_path, *HOME_ALIAS))
    {
        algo::replace(t_path, HOME_ALIAS, user_home_path);
    }
    return t_path;
}

/**
* @brief
*     Replace the home alias (`~`) in the given file path
*     with the resolved user home profile directory path.
*/
std::string scan::path::replace_home_alias(const string& t_path)
{
    string buffer{t_path};
    return replace_home_alias(buffer);
}

/**
* @brief
*     Resolve the given file path.
*/
std::string& scan::path::resolve(string& t_path)
{
    if (!t_path.empty())
    {
        t_path = resolve_path(t_path).string();
    }
    return normalize(t_path);
}

/**
* @brief
*     Resolve the given file path.
*/
std::string scan::path::resolve(const string& t_path)
{
    string buffer{t_path};
    return resolve(buffer);
}

/**
* @brief
*     Replace the home alias (`~`) in the given standard library
*     file path with the resolved user home profile directory path.
*/
scan::file_path_t& scan::path::replace_home_alias_path(file_path_t& t_file_path)
{
    return t_file_path = replace_home_alias(t_file_path.string());
}

/**
* @brief
*     Replace the home alias (`~`) in the given standard library
*     file path with the resolved user home profile directory path.
*/
scan::file_path_t scan::path::replace_home_alias_path(const file_path_t& t_file_path)
{
    file_path_t buffer{t_file_path};
    return replace_home_alias_path(buffer);
}

/**
* @brief
*     Resolve the given standard library file path.
*/
scan::file_path_t& scan::path::resolve_path(file_path_t& t_file_path)
{
    replace_home_alias_path(t_file_path);
    return t_file_path = filesystem::absolute(t_file_path);
}

/**
* @brief
*     Resolve the given standard library file path.
*/
scan::file_path_t scan::path::resolve_path(const file_path_t& t_file_path)
{
    file_path_t buffer{t_file_path};
    return resolve_path(buffer);
}
