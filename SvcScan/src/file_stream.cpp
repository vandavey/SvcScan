/*
*  file_stream.cpp
*  ---------------
*  Source file for a system file stream
*/
#include "includes/except/runtime_ex.h"
#include "includes/io/filesys/file_stream.h"

/**
* @brief  Initialize the object.
*/
scan::FileStream::FileStream()
{
    mode = write_mode();
}

/**
* @brief  Initialize the object.
*/
scan::FileStream::FileStream(FileStream &&t_fstream) noexcept
{
    *this = std::move(t_fstream);
}

/**
* @brief  Initialize the object.
*/
scan::FileStream::FileStream(const string &t_path, const openmode &t_mode)
{
    if (!Path::valid_file(t_path))
    {
        throw ArgEx{ "t_path", "The given file path is invalid" };
    }

    path = Path::resolve(t_path);
    mode = t_mode;

    open(Path::resolve(t_path), t_mode);
}

/**
* @brief  Destroy the object.
*/
scan::FileStream::~FileStream()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

/**
* @brief  Move assignment operator overload.
*/
scan::FileStream &scan::FileStream::operator=(FileStream &&t_fstream) noexcept
{
    if (this != &t_fstream)
    {
        m_file = std::move(t_fstream.m_file);
        mode = t_fstream.mode;
        path = t_fstream.path;
    }
    return *this;
}

/**
* @brief  Bitwise right shift operator overload.
*/
std::istream &scan::FileStream::operator>>(string &t_buffer)
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::operator>>", "Underlying file is closed" };
    }
    return m_file >> t_buffer;
}

/**
* @brief  Write all the given data to the specified file path and close the stream.
*/
void scan::FileStream::write(const string &t_path,
                             const string &t_data,
                             const bool &t_binary) {

    this_t(t_path, write_mode(t_binary)).write(t_data, true);
}

/**
* @brief  Get the default file stream open mode for read operations.
*/
std::fstream::openmode scan::FileStream::read_mode(const bool &t_binary) noexcept
{
    return t_binary ? fstream::in | fstream::binary : fstream::in;
}

/**
* @brief  Get the default file stream open mode for write operations.
*/
std::fstream::openmode scan::FileStream::write_mode(const bool &t_binary) noexcept
{
    openmode mode{ fstream::out | fstream::trunc | fstream::binary };

    if (!t_binary)
    {
        mode ^= fstream::binary;
    }
    return mode;
}

/**
* @brief  Read all the data from the given file path and close the stream.
*/
std::string scan::FileStream::read(const string &t_path, const bool &t_binary)
{
    return this_t(t_path, read_mode(t_binary)).read(true);
}

/**
* @brief  Close the underlying file stream.
*/
void scan::FileStream::close()
{
    if (is_open())
    {
        m_file.close();
    }
}

/**
* @brief  Open the underlying file stream using the underlying file
*         path and open mode.
*/
void scan::FileStream::open()
{
    open(path, mode);
}

/**
* @brief  Open the underlying file stream using the given file path
*         and specified open mode.
*/
void scan::FileStream::open(const string &t_path, const openmode &t_mode)
{
    if (!Path::valid_file(t_path))
    {
        throw LogicEx{ "FileStream::open", "Invalid underlying file path" };
    }

    path = Path::resolve(t_path);
    mode = t_mode;

    m_file.open(path, mode);
    throw_if_failed();
}

/**
* @brief  Determine whether the underlying file stream is open.
*/
bool scan::FileStream::is_open() const
{
    return m_file.is_open();
}

/**
* @brief Determine the size of the underlying file stream (in bytes).
*/
std::streamsize scan::FileStream::size(const bool &t_close)
{
    if (!is_open())
    {
        throw LogicEx{ "FileStream::size", "Underlying file is closed" };
    }

    filebuf *bufferp{ m_file.rdbuf() };
    throw_if_failed();

    // Seek to EOF position
    const streamsize file_size{ bufferp->pubseekoff(0, fstream::end, mode) };

    // Rewind to BOF position
    bufferp->pubseekoff(0, fstream::beg, mode);

    return file_size;
}

/**
* @brief  Read all the data from the underlying file stream and
*         optionally close the stream.
*/
std::string scan::FileStream::read(const bool &t_close)
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
        file_data = string(static_cast<size_t>(file_size), '\0');

        m_file.rdbuf()->sgetn(&file_data[0], file_size);
        throw_if_failed();
    }

    if (t_close)
    {
        close();
        throw_if_failed();
    }
    return file_data;
}

/**
* @brief  Throw a runtime exception if any of the error bits are set
*         in the underlying file stream.
*/
void scan::FileStream::throw_if_failed() const
{
    if (!m_file.good())
    {
        const string caller{ "FileStream::throw_if_failed" };
        throw RuntimeEx{ caller, "Error occurred in the underlying file stream" };
    }
}
