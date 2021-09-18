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
/// Bitwise right shift operator overload
/// ***
std::istream &scan::FileStream::operator>>(string &t_buffer)
{
    if (!is_open())
    {
        throw LogicEx("FileStream::operator>>", "Underlying file must be open");
    }
    return (m_file >> t_buffer);
}

/// ***
/// Utility - Read all the text from the given CSV path and close stream
/// ***
std::string scan::FileStream::read_csv(const string &t_path)
{
    FileStream file{ t_path, fstream::in };
    return file.read_csv(true);
}

/// ***
/// Utility - Read all the text of an embedded CSV file resource
/// ***
std::string scan::FileStream::read_csv(const TextRc &t_rc)
{
    return t_rc.data();
}

/// ***
/// Utility - Read all the lines from the given CSV path and close stream
/// ***
scan::FileStream::vector_s scan::FileStream::read_csv_lines(const string &t_path)
{
    FileStream file{ t_path, fstream::in };
    return file.read_csv_lines(true);
}

/// ***
/// Utility - Read all the lines of an embedded CSV file resource
/// ***
scan::FileStream::vector_s scan::FileStream::read_csv_lines(const TextRc &t_rc)
{
    return Util::split(t_rc.data(), stdu::LF);
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
/// Read all text from the given CSV path and close stream
/// ***
std::string scan::FileStream::read_csv(const bool &t_close)
{
    if (!is_open())
    {
        throw LogicEx("FileStream::read_csv", "Underlying file must be open");
    }

    string buffer;
    std::stringstream ss;

    // Read data until EOF detected
    while (m_file >> buffer)
    {
        ss << buffer;
        (buffer[buffer.size() - 1] == '"') ? (ss << stdu::LF) : (ss << " ");
    }

    if (t_close)
    {
        close();
    }
    return ss.str();
}

/// ***
/// Read all lines from the given CSV path and close stream
/// ***
scan::FileStream::vector_s scan::FileStream::read_csv_lines(const bool &t_close)
{
    if (!is_open())
    {
        throw LogicEx("FileStream::read_csv_lines", "Underlying file must be open");
    }
    return Util::split(read_csv(t_close), stdu::LF);
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
