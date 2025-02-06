/*
* @file
*     file.cpp
* @brief
*     Source file for a system file stream.
*/
#include "includes/errors/arg_ex.h"
#include "includes/file_system/file.h"
#include "includes/file_system/path.h"
#include "includes/file_system/path_info.h"

/**
* @brief
*     Initialize the object.
*/
scan::File::File() noexcept
{
    m_eol = Eol::lf;
    m_mode = default_mode();
}

/**
* @brief
*     Initialize the object.
*/
scan::File::File(const string& t_path, openmode t_mode, Eol t_eol)
{
    m_eol = t_eol;
    open(t_path, t_mode);
}

/**
* @brief
*     Read all data from the given file path and close the stream. Line-endings in
*     the resulting data will be normalized using the specified EOL control sequence.
*/
std::string scan::File::read(const string& t_path, Eol t_eol)
{
    File file{t_path, default_read_mode(), t_eol};

    const string data{file.read()};
    file.close();

    return data;
}

/**
* @brief
*     Close the underlying file stream.
*/
void scan::File::close()
{
    if (is_open())
    {
        m_fstream.close();
    }
}

/**
* @brief
*     Open the underlying file stream using the underlying file path and open mode.
*/
void scan::File::open()
{
    open(m_path, m_mode);
}

/**
* @brief
*     Open the underlying file stream using the given file path and specified open mode.
*/
void scan::File::open(const string& t_path, openmode t_mode)
{
    if (!path::file_or_parent_exists(t_path))
    {
        throw ArgEx{INVALID_PATH_MSG, "t_path"};
    }

    if (path::path_info(t_path) == PathInfo::new_file && read_only_permitted(t_mode))
    {
        throw ArgEx{FILE_NOT_FOUND_MSG, "t_path"};
    }

    m_path = path::resolve(t_path);
    m_mode = t_mode;
    m_fstream.open(m_path, m_mode);

    if (!is_open())
    {
        throw ArgEx{FILE_OPEN_FAILED_MSG, "t_path"};
    }
}

/**
* @brief
*     Determine whether the underlying file stream is open.
*/
bool scan::File::is_open() const noexcept
{
    return m_fstream.is_open();
}

/**
* @brief
*     Read all data from the underlying file stream. Line-endings in the resulting
*     data will be normalized using the underlying EOL control sequence.
*/
std::string scan::File::read()
{
    if (!is_open())
    {
        throw LogicEx{FILE_CLOSED_MSG, "File::read"};
    }

    if (!read_permitted(m_mode))
    {
        throw LogicEx{FILE_OP_UNPERMITTED_MSG, "File::read"};
    }
    return algo::normalize_eol(m_fstream.rdbuf());
}
