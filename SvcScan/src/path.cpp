/*
*  path.h
*  ------
*  Source file for file system and path utilities
*/
#include <fstream>
#include "includes/except/argex.h"
#include "includes/utils/path.h"
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
/// Determine if the given file path exists and is valid
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
    if (t_path.empty() || fspath(t_path).is_absolute())
    {
        return t_path;
    }
    const size_t end_pos{ t_path.size() - 1 };

    // Determine if argument ends with path separator
    const bool ew_sep{ ends_with(t_path, vector_s{ "/", "\\" }) };

    // Remove terminating path separator
    if (ew_sep && (t_path.size() > 1))
    {
        return fs::absolute(t_path.substr(0, end_pos)).string();
    }
    return fs::absolute(t_path).string();
}

/// ***
/// Determine if the given path ends with the substring
/// ***
bool scan::Path::ends_with(const string &t_path, const string &t_sub)
{
    if (t_path.empty())
    {
        return false;
    }
    return t_path.rfind(t_sub) == (t_path.size() - 1);
}

/// ***
/// Determine if the given path ends with one or more substrings
/// ***
bool scan::Path::ends_with(const string &t_path, const vector_s &t_svect)
{
    if (t_path.empty())
    {
        return false;
    }

    // Check each substring for a match
    return std::all_of(t_svect.begin(), t_svect.end(), [t_path](const string &l_sub)
    {
        return ends_with(t_path, l_sub);
    });
}
