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
/// Create a new file or truncate an existing file
/// ***
bool scan::Path::create_file(const string &t_spath)
{
    // Invalid file path argument
    if (t_spath.empty() || !is_valid(t_spath))
    {
        throw ArgEx("t_spath", "Invalid file path argument");
    }
    const string full_path{ resolve(t_spath) };

    // Create/truncate file
    std::ofstream ofs{ full_path };
    ofs.close();

    return exists(full_path);
}

/// ***
/// Determine if the given file path exists
/// ***
bool scan::Path::exists(const string &t_spath)
{
    if (t_spath.empty())
    {
        return false;
    }
    return filesystem::exists(resolve(t_spath));
}

/// ***
/// Determine if the given file path leads to a directory
/// ***
bool scan::Path::is_dir(const string &t_spath)
{
    if (t_spath.empty())
    {
        return false;
    }
    return filesystem::is_directory(resolve(t_spath));
}

/// ***
/// Determine if the given file path exists and is valid
/// ***
bool scan::Path::is_valid(const string &t_spath)
{
    const string full_path{ resolve(t_spath) };

    // Path cannot be empty or lead to a directory
    if (full_path.empty() || is_dir(full_path))
    {
        return false;
    }

    // Existing file path located
    if (exists(full_path))
    {
        return true;
    }

    // Parent directory not found
    if (!exists(parent(full_path)))
    {
        return false;
    }
    return true;
}

/// ***
/// Retrieve the parent directory of the given file path
/// ***
std::string scan::Path::parent(const string &t_spath)
{
    if (t_spath.empty())
    {
        return t_spath;
    }
    return path(resolve(t_spath)).parent_path().string();
}

/// ***
/// Resolve the absolute file of the given relative path
/// ***
std::string scan::Path::resolve(const string &t_spath)
{
    if (t_spath.empty() || path(t_spath).is_absolute())
    {
        return t_spath;
    }
    const size_t end_pos{ t_spath.size() - 1 };

    // Determine if argument ends with path separator
    const bool ew_sep{ ends_with(t_spath, vector_s{ "/", "\\" }) };

    // Remove terminating path separator
    if (ew_sep && (t_spath.size() > 1))
    {
        return filesystem::absolute(t_spath.substr(0, end_pos)).string();
    }
    return filesystem::absolute(t_spath).string();
}

/// ***
/// Determine if the given path ends with the substring
/// ***
bool scan::Path::ends_with(const string &t_spath, const string &t_sub)
{
    if (t_spath.empty())
    {
        return false;
    }
    return t_spath.rfind(t_sub) == (t_spath.size() - 1);
}

/// ***
/// Determine if the given path ends with one or more substrings
/// ***
bool scan::Path::ends_with(const string &t_spath, const vector_s &t_svect)
{
    if (t_spath.empty())
    {
        return false;
    }

    // Check each substring for a match
    return std::all_of(t_svect.begin(), t_svect.end(), [t_spath](const string &t_sub)
    {
        return ends_with(t_spath, t_sub);
    });
}
