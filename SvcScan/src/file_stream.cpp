/*
*  file_stream.cpp
*  ---------------
*  Source file for a system file stream
*/
#include "includes/filesys/file_stream.h"

/// ***
/// Initialize the object
/// ***
scan::FileStream::FileStream()
{
    mode = fstream::out | fstream::trunc;
}

/// ***
/// Initialize the object
/// ***
scan::FileStream::FileStream(const string &t_path,
                             const openmode &t_mode) : path(t_path), mode(t_mode) {
    // Invalid file path
    if (!Path::valid_file(t_path))
    {
        throw ArgEx{ "t_path", "The given file path is invalid" };
    }

    // Invalid open mode
    if (!valid_mode(t_mode))
    {
        throw ArgEx{ "t_mode", "The given file open mode is invalid" };
    }

    path = Path::resolve(t_path);
    open(t_mode);
}

/// ***
/// Destroy the object
/// ***
scan::FileStream::~FileStream()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

/// ***
/// Bitwise right shift operator overload
/// ***
std::istream &scan::FileStream::operator>>(string &t_buffer)
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::operator>>", "Underlying file closed" };
    }
    return (m_file >> t_buffer);
}

/// ***
/// Utility - Read all the text from the given text file path and close stream
/// ***
std::string scan::FileStream::read_text(const string &t_path)
{
    FileStream file{ t_path, fstream::in };
    return file.read_text(true);
}

/// ***
/// Utility - Read all the lines of an embedded text file resource and close stream
/// ***
std::vector<std::string> scan::FileStream::read_lines(const string &t_path)
{
    FileStream file{ t_path, fstream::in };
    return file.read_lines(true);
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
/// Open the underlying file stream using the given open mode
/// ***
void scan::FileStream::open(const openmode &t_mode)
{
    if (!valid_mode(t_mode))
    {
        throw ArgEx{ "t_mode", "The given file open mode is invalid" };
    }

    if (!Path::valid_file(path))
    {
        throw LogicEx{ "FileStream::open", "Invalid underlying file path" };
    }

    // Open the underlying file stream
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
/// Determine the size of a file using the underlying file stream
/// ***
std::streamsize scan::FileStream::size(const bool &t_close)
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::size", "Underlying file closed" };
    }
    filebuf *filebufp{ m_file.rdbuf() };

    // Seek to EOF position
    const streamsize fsize{ filebufp->pubseekoff(0, fstream::end, mode) };

    // Rewind to BOF position
    filebufp->pubseekoff(0, fstream::beg, mode);

    return fsize;
}

/// ***
/// Read all the data from a text file using the underlying path
/// ***
std::string scan::FileStream::read_text(const bool &t_close)
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::read_text", "Underlying file closed" };
    }

    string fdata;
    const streamsize fsize{ size() };

    // Read the file data
    if (fsize != INVALID_SIZE)
    {
        fdata = string(static_cast<uint>(fsize), '\0');
        m_file.rdbuf()->sgetn(&fdata[0], fsize);
    }

    if (t_close)
    {
        close();
    }
    return fdata;
}

/// ***
/// Read all the lines from a text file using the underlying path
/// ***
std::vector<std::string> scan::FileStream::read_lines(const bool &t_close)
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::read_lines", "Underlying file closed" };
    }
    return Util::split(read_text(t_close), stdu::LF);
}

/// ***
/// Determine if file open mode integer is valid
/// ***
bool scan::FileStream::valid_mode(const openmode &t_mode)
{
    // Valid file open modes
    const List<openmode> open_modes
    {
        fstream::app,
        fstream::in,
        fstream::out,
        fstream::trunc,
        fstream::out | fstream::app,
        fstream::out | fstream::trunc
    };
    return open_modes.contains(t_mode);
}
