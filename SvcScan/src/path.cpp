/*
*  path.h
*  ------
*  Source file for file system and path utilities
*/
#include <fstream>
#include "includes/except/argex.h"
#include "includes/filesys/path.h"
#include "includes/utils/util.h"

/// ***
/// Determine if the given file path exists
/// ***
bool scan::Path::exists(const string &t_path)
{
    return t_path.empty() ? false : fs::exists(resolve(t_path));
}

/// ***
/// Determine if the given file path is absolute
/// ***
bool scan::Path::is_absolute(const string &t_path)
{
    return t_path.empty() ? false : fspath(resolve(t_path)).is_absolute();
}

/// ***
/// Determine if the given file path leads to a directory
/// ***
bool scan::Path::is_directory(const string &t_path)
{
    return t_path.empty() ? false : fs::is_directory(resolve(t_path));
}

/// ***
/// Determine if the given file path is valid
/// ***
bool scan::Path::valid_file(const string &t_path)
{
    const string full_path{ resolve(t_path) };
    const PathInfo info{ path_info(full_path) };

    return (info == PathInfo::parent_exists) || (info == PathInfo::exists);
}

/// ***
/// Retrieve information for the given file path
/// ***
scan::PathInfo scan::Path::path_info(const string &t_path)
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

/// ***
/// Retrieve the parent directory of the given file path
/// ***
std::string scan::Path::parent(const string &t_path)
{
    return t_path.empty() ? t_path : fspath(resolve(t_path)).parent_path().string();
}

/// ***
/// Resolve the absolute file of the given relative path
/// ***
std::string scan::Path::resolve(const string &t_path)
{
    string path;

    // Do not resolve empty or absolute paths
    if (fspath(t_path).is_absolute())
    {
        path = t_path;
    }
    else if (!t_path.empty())
    {
        vector_s path_parts{ parts(t_path) };

        // Resolve user home path
        if (path_parts[0] == "~")
        {
            path_parts[0] = user_home();
        }
        path = normalize(list_s::join(path_parts, "/"));
    }

    return path;
}

/// ***
/// Return a vector containing all of the given file path's elements
/// ***
scan::Path::vector_s scan::Path::parts(const string &t_path)
{
    vector_s parts;

    if (!t_path.empty())
    {
        parts = Util::split(normalize(t_path), "/");
    }
    return parts;
}

/// ***
/// Normalize the given file path's separators and formatting
/// ***
std::string scan::Path::normalize(const string &t_path)
{
    string path;

    if (!t_path.empty())
    {
        path = Util::replace(t_path, R"(\\)", "/");

        // Remove trailing path separator
        if (Util::ends_with(path, "/"))
        {
            path = path.substr(0, path.size() - 1);
        }
    }
    return path;
}

/// ***
/// Retrieve the current user's home directory file path
/// ***
std::string scan::Path::user_home(const string &t_env_var)
{
    string path;

    if (!t_env_var.empty())
    {
        size_t size_required;

        // Calculate required buffer size
        getenv_s(&size_required, nullptr, 0, t_env_var.c_str());

        // Get environment variable value
        if (size_required > 0)
        {
            path = string(size_required, '\0');
            getenv_s(&size_required, &path[0], size_required, t_env_var.c_str());
        }
    }
    return normalize(path);
}
