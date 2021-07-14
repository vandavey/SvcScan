/*
*  streamer.cpp
*  ------------
*  Source file for handling file stream data and operations
*/
#include "includes/container/list.h"
#include "includes/except/argex.h"
#include "includes/except/logicex.h"
#include "includes/utils/filestream.h"
#include "includes/utils/path.h"

/// Default output file path
scan::AutoProp<std::string> scan::FileStream::default_path{ string() };

/// ***
/// Initialize the object
/// ***
scan::FileStream::FileStream()
{
    path = default_path;
}

/// ***
/// Initialize the object
/// ***
scan::FileStream::FileStream(const string &t_path,
                             const openmode &t_mode) : path(t_path), mode(t_mode) {
    if (!valid_mode(t_mode))
    {
        throw ArgEx("t_mode", "The given file open mode is invalid");
    }

    // Use default path if set
    if (!default_path.get().empty() || t_path.empty())
    {
        path = default_path;
    }

    if (!Path::valid_file(t_path))
    {
        throw ArgEx("t_path", "The given file path is invalid");
    }

    if (!Path::is_absolute(t_path))
    {
        path = Path::resolve(t_path);
    }

    m_file.open(path, t_mode);
}

/// ***
/// Close the underlying output file stream
/// ***
void scan::FileStream::close()
{
    if (is_open())
    {
        m_file.close();
    }
}

/// ***
/// Open the given file using the given open mode
/// ***
void scan::FileStream::open(const string &t_path, const openmode &t_mode)
{
    const string file_path{ t_path.empty() ? default_path.get() : t_path };
    const PathInfo info{ Path::path_info(file_path) };

    // Invalid  file path
    if (!Path::valid_file(file_path))
    {
        throw ArgEx("t_path", "Invalid output file path");
    }

    // File path leads to directory
    if (Path::is_directory(t_path))
    {
        throw ArgEx("t_path", "Given or default file path leads to a directory");
    }

    // Invalid file open mode
    if (!valid_mode(t_mode))
    {
        throw ArgEx("t_mode", "The given file open mode is invalid");
    }

    //bool failed{ m_file.fail() };

    m_file.open(file_path, mode);
}

/// ***
/// Determine if the underlying output file stream is open
/// ***
bool scan::FileStream::is_open() const
{
    return m_file.is_open();
}

/// ***
/// Determine if file open mode integer is valid
/// ***
bool scan::FileStream::valid_mode(const openmode &t_mode)
{
    // Valid file open modes
    const List<openmode> open_modes
    {
        ofstream::out,
        ofstream::app,
        ofstream::trunc,
        ofstream::out | ofstream::app,
        ofstream::out | ofstream::trunc
    };
    return open_modes.contains(t_mode);
}
