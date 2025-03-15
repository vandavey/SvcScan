/*
* @file
*     path.cpp
* @brief
*     Source file for file system and path utilities.
*/
#include <filesystem>
#include <system_error>
#include <utility>
#include "includes/file_system/path.h"
#include "includes/utils/const_defs.h"

/**
* @brief
*     Determine whether the given file path exists.
*/
bool scan::path::exists(const string& t_path)
{
    error_code ecode;
    return !t_path.empty() && filesystem::exists(resolve(t_path), ecode);
}

/**
* @brief
*     Determine whether the given file system error code is indicative of an error.
*/
bool scan::path::is_error(const error_code& t_ecode) noexcept
{
    return t_ecode.value() != RCODE_NO_ERROR;
}

/**
* @brief
*     Determine whether the given file system error is indicative of an error.
*/
bool scan::path::is_error(const filesystem_error& t_error) noexcept
{
    return is_error(t_error.code());
}

/**
* @brief
*     Determine whether the given standard library file path exists.
*/
bool scan::path::path_exists(const path_t& t_file_path)
{
    error_code ecode;
    return !t_file_path.empty() && filesystem::exists(resolve_path(t_file_path), ecode);
}

/**
* @brief
*     Get information about the given file path.
*/
scan::PathInfo scan::path::path_info(const path_t& t_file_path)
{
    PathInfo info;
    error_code ecode;

    const path_t file_path{resolve_path(t_file_path)};

    switch (filesystem::status(file_path, ecode).type())
    {
        case file_type::none:
            info = PathInfo::empty;
            break;
        case file_type::not_found:
            info = path_info_not_found(file_path);
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
scan::PathInfo scan::path::path_info_not_found(const path_t& t_file_path)
{
    PathInfo info{PathInfo::not_found};
    const path_t file_path{resolve_path(t_file_path)};

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
scan::path_t& scan::path::replace_home_alias_path(path_t& t_file_path)
{
    return t_file_path = replace_home_alias(t_file_path.string());
}

/**
* @brief
*     Replace the home alias (`~`) in the given standard library
*     file path with the resolved user home profile directory path.
*/
scan::path_t scan::path::replace_home_alias_path(const path_t& t_file_path)
{
    path_t buffer{t_file_path};
    return replace_home_alias_path(buffer);
}

/**
* @brief
*     Resolve the given standard library file path.
*/
scan::path_t& scan::path::resolve_path(path_t& t_file_path)
{
    replace_home_alias_path(t_file_path);
    return t_file_path = filesystem::absolute(t_file_path);
}

/**
* @brief
*     Resolve the given standard library file path.
*/
scan::path_t scan::path::resolve_path(const path_t& t_file_path)
{
    path_t buffer{t_file_path};
    return resolve_path(buffer);
}

/**
* @brief
*     Create a valid file system error.
*/
scan::filesystem_error scan::path::make_error()
{
    return filesystem_error{string{}, error_code{}};
}

/**
* @brief
*     Create a file system error with the given error message.
*/
scan::filesystem_error scan::path::make_error(const string& t_msg)
{
    return filesystem_error{t_msg, std::make_error_code(errc::io_error)};
}

/**
* @brief
*     Create a file system error with the given error message for the specified file path.
*/
scan::filesystem_error scan::path::make_error(const string& t_msg,
                                              const path_t& t_file_path)
{
    return make_error(t_msg, t_file_path, std::make_error_code(errc::io_error));
}

/**
* @brief
*     Create a file system error with the given error details for the specified file path.
*/
scan::filesystem_error scan::path::make_error(const string& t_msg,
                                              const path_t& t_file_path,
                                              error_code&& t_ecode)
{
    return filesystem_error{t_msg, t_file_path, std::move(t_ecode)};
}

/**
* @brief
*     Reset the given file system error.
*/
scan::filesystem_error& scan::path::reset_error(filesystem_error& t_error)
{
    if (is_error(t_error))
    {
        t_error = std::move(make_error());
    }
    return t_error;
}
