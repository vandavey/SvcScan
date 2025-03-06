/*
* @file
*     file.cpp
* @brief
*     Source file for a system file stream.
*/
#include <chrono>
#include <utility>
#include "includes/file_system/file.h"

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
*     Create a new empty file or update the timestamp of
*     the existing file located at at the given file path.
*/
bool scan::File::touch(const path_t& t_file_path)
{
    filesystem_error error{path::make_error()};
    return touch(t_file_path, error);
}

/**
* @brief
*     Create a new empty file or update the timestamp of
*     the existing file located at at the given file path.
*/
bool scan::File::touch(const path_t& t_file_path, filesystem_error& t_error)
{
    const path_t file_path{path::resolve_path(t_file_path)};
    const PathInfo info{path::path_info(file_path)};

    if (info == PathInfo::new_file)
    {
        File file{file_path, default_write_mode(), t_error};
        file.close();
    }
    else if (info == PathInfo::file)
    {
        error_code ecode;
        filesystem::last_write_time(file_path, chrono::file_clock::now(), ecode);

        // Update error code reference
        if (path::is_error(ecode))
        {
            t_error = path::make_error(FILE_WRITE_FAILED, file_path, std::move(ecode));
        }
        else  // Reset error reference
        {
            path::reset_error(t_error);
        }
    }

    return !path::is_error(t_error);
}

/**
* @brief
*     Read all data from the given file path and close the stream. Line-endings in
*     the resulting data will be normalized using the specified EOL control sequence.
*/
std::string scan::File::read(const path_t& t_file_path, Eol t_eol)
{
    File file{t_file_path, default_read_mode(), t_eol};

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
*     Determine whether an error occurred in the underlying file stream.
*/
bool scan::File::fail() const noexcept
{
    return m_fstream.fail();
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
    filesystem_error error{path::make_error()};
    const string data{read(error)};

    if (path::is_error(error))
    {
        throw RuntimeEx{error.what(), "File::read"};
    }
    return data;
}

/**
* @brief
*     Read all data from the underlying file stream. Line-endings in the resulting
*     data will be normalized using the underlying EOL control sequence.
*/
std::string scan::File::read(filesystem_error& t_error)
{
    string data;

    if (!is_open())
    {
        t_error = path::make_error(FILE_CLOSED_MSG);
    }
    else if (fail())
    {
        t_error = path::make_error(FILE_FAIL_STATE_MSG);
    }
    else if (!path::read_permitted(m_mode))
    {
        t_error = path::make_error(FILE_OP_UNPERMITTED_MSG);
    }
    else  // Read data from file stream
    {
        data = algo::normalize_eol(m_fstream.rdbuf());
    }

    return data;
}
