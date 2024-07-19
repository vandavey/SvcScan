/*
* @file
*     file_stream.cpp
* @brief
*     Source file for a system file stream.
*/
#include <ios>
#include <iosfwd>
#include <string>
#include "includes/errors/arg_ex.h"
#include "includes/io/filesys/file_stream.h"
#include "includes/io/filesys/path.h"
#include "includes/utils/const_defs.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Initialize the object.
*/
scan::FileStream::FileStream() noexcept
{
    m_mode = default_mode();
}

/**
* @brief
*     Initialize the object.
*/
scan::FileStream::FileStream(FileStream&& t_fstream) noexcept
{
    *this = std::move(t_fstream);
}

/**
* @brief
*     Initialize the object.
*/
scan::FileStream::FileStream(const string& t_path, const openmode& t_mode)
{
    if (!path::file_or_parent_exists(t_path))
    {
        throw ArgEx{ "t_path", "The given file path is invalid" };
    }

    m_path = path::resolve(t_path);
    m_mode = t_mode;

    open(path::resolve(t_path), t_mode);
}

/**
* @brief
*     Bitwise right shift operator overload.
*/
std::istream& scan::FileStream::operator>>(string& t_buffer)
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::operator>>", "Underlying file is closed" };
    }
    return m_file >> t_buffer;
}

/**
* @brief
*     Write all the given data to the specified file path and close the stream.
*/
void scan::FileStream::write(const string& t_path, const string& t_data)
{
    FileStream stream{ t_path, default_write_mode() };

    stream.write(t_data);
    stream.close();
}

/**
* @brief
*     Read all the data from the given file path and close the stream.
*/
std::string scan::FileStream::read(const string& t_path)
{
    FileStream stream{ t_path, default_read_mode() };

    const string data{ stream.read() };
    stream.close();

    return data;
}

/**
* @brief
*     Close the underlying file stream.
*/
void scan::FileStream::close()
{
    if (is_open())
    {
        m_file.close();
    }
}

/**
* @brief
*     Open the underlying file stream using the underlying file path and open mode.
*/
void scan::FileStream::open()
{
    open(m_path, m_mode);
}

/**
* @brief
*     Open the underlying file stream using the given file path and specified open mode.
*/
void scan::FileStream::open(const string& t_path, const openmode& t_mode)
{
    if (!path::file_or_parent_exists(t_path))
    {
        throw ArgEx{ "t_path", "The given file path is invalid" };
    }

    m_path = path::resolve(t_path);
    m_mode = t_mode;

    m_file.open(m_path, m_mode);
}

/**
* @brief
*     Determine whether the underlying file stream is open.
*/
bool scan::FileStream::is_open() const noexcept
{
    return m_file.is_open();
}

/**
* @brief
*     Determine the size of the underlying file stream (in bytes).
*/
std::streamsize scan::FileStream::size()
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::size", "Underlying file is closed" };
    }

    std::filebuf* bufferp{ m_file.rdbuf() };

    // Seek to EOF position
    const streamsize file_size{ bufferp->pubseekoff(0_i64, fstream::end, m_mode) };

    // Rewind to BOF position
    bufferp->pubseekoff(0_i64, fstream::beg, m_mode);

    return file_size;
}

/**
* @brief
*     Read all the data from the underlying file stream and optionally close the stream.
*/
std::string scan::FileStream::read()
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::read", "Underlying file is closed" };
    }

    string file_data;
    const streamsize file_size{ size() };

    // Read the file data
    if (file_size != INVALID_SIZE)
    {
        file_data = string(static_cast<size_t>(file_size), CHAR_NULL);
        m_file.rdbuf()->sgetn(&file_data[0], file_size);
    }
    return file_data;
}
