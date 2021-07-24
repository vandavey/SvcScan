/*
*  filestream.cpp
*  --------------
*  Source file for handling file stream data and operations
*/
#include "includes/containers/list.h"
#include "includes/except/argex.h"
#include "includes/except/logicex.h"
#include "includes/filesys/filestream.h"
#include "includes/filesys/path.h"

/// ***
/// Initialize the object
/// ***
scan::FileStream::FileStream(const string &t_path,
                             const openmode &t_mode) : path(t_path), mode(t_mode) {
    // Invalid file path
    if (!Path::valid_file(t_path))
    {
        throw ArgEx("t_path", "The given file path is invalid");
    }

    // Invalid open mode
    if (!valid_mode(t_mode))
    {
        throw ArgEx("t_mode", "The given file open mode is invalid");
    }

    path = Path::resolve(t_path);
    open(t_mode);
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
void scan::FileStream::open(const openmode &t_mode)
{
    if (!valid_mode(t_mode))
    {
        throw ArgEx("t_mode", "The given file open mode is invalid");
    }

    if (!Path::valid_file(path))
    {
        throw LogicEx("FileStream::open", "Invalid underlying file path");
    }

    // Open underlying file stream
    m_file.open(Path::resolve(path), mode);
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
        ofs::out,
        ofs::app,
        ofs::trunc,
        ofs::out | ofs::app,
        ofs::out | ofs::trunc
    };
    return open_modes.contains(t_mode);
}
