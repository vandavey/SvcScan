/*
* @file
*     file.h
* @brief
*     Header file for a system file stream.
*/
#pragma once

#ifndef SCAN_FILE_H
#define SCAN_FILE_H

#include <filesystem>
#include <fstream>
#include <string>
#include "../concepts/concepts.h"
#include "../errors/error_const_defs.h"
#include "../errors/runtime_ex.h"
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "../utils/eol.h"
#include "file_system_aliases.h"
#include "path.h"
#include "path_info.h"

namespace scan
{
    /**
    * @brief
    *     System file stream.
    */
    class File
    {
    private:  /* Fields */
        Eol m_eol;           // EOL control sequence
        open_mode_t m_mode;  // File open mode

        string m_path;       // File path
        fstream m_fstream;   // File stream

    public:  /* Constructors & Destructor */
        File() noexcept;
        File(const File&) = delete;
        File(File&&) = default;

        File(const path_t& t_file_path,
             BitMask auto t_mode = default_mode(),
             Eol t_eol = Eol::lf);

        File(const path_t& t_file_path,
             BitMask auto t_mode,
             filesystem_error& t_error,
             Eol t_eol = Eol::lf);

        virtual ~File() = default;

    public:  /* Operators */
        File& operator=(const File&) = delete;
        File& operator=(File&&) = default;

        File& operator<<(const LShift auto& t_data);

    public:  /* Methods */
        static void write(const path_t& t_file_path,
                          const LShift auto& t_data,
                          Eol t_eol = Eol::lf);

        static bool touch(const path_t& t_file_path);
        static bool touch(const path_t& t_file_path, filesystem_error& t_error);

        static string read(const path_t& t_file_path, Eol t_eol = Eol::lf);

        void close();
        void open(const path_t& t_file_path, BitMask auto t_mode);

        void open(const path_t& t_file_path,
                  BitMask auto t_mode,
                  filesystem_error& t_error);

        void write(const LShift auto& t_data);
        void write(const LShift auto& t_data, filesystem_error& t_error);

        bool fail() const noexcept;
        bool is_open() const noexcept;

        string read();
        string read(filesystem_error& t_error);
    };
}

/**
* @brief
*     Initialize the object.
*/
inline scan::File::File(const path_t& t_file_path, BitMask auto t_mode, Eol t_eol)
    : m_eol{t_eol}
{
    open(t_file_path, t_mode);
}

/**
* @brief
*     Initialize the object.
*/
inline scan::File::File(const path_t& t_file_path,
                        BitMask auto t_mode,
                        filesystem_error& t_error,
                        Eol t_eol)
    : m_eol{t_eol}
{
    open(t_file_path, t_mode, t_error);
}

/**
* @brief
*     Bitwise left shift operator overload.
*/
inline scan::File& scan::File::operator<<(const LShift auto& t_data)
{
    write(t_data);
    return *this;
}

/**
* @brief
*     Write the given data to the specified file path and close the stream. Line-endings
*     in the data will be normalized using the specified EOL control sequence.
*/
inline void scan::File::write(const path_t& t_file_path,
                              const LShift auto& t_data,
                              Eol t_eol)
{
    File file{t_file_path, path::default_write_mode(), t_eol};

    file.write(t_data);
    file.close();
}

/**
* @brief
*     Open the underlying file stream using the given file path and specified open mode.
*/
inline void scan::File::open(const path_t& t_file_path, BitMask auto t_mode)
{
    filesystem_error error{path::make_error()};
    open(t_file_path, t_mode, error);

    if (path::is_error(error))
    {
        throw RuntimeEx{error.what(), "File::open"};
    }
}

/**
* @brief
*     Open the underlying file stream using the given file path and specified open mode.
*/
inline void scan::File::open(const path_t& t_file_path,
                             BitMask auto t_mode,
                             filesystem_error& t_error)
{
    const PathInfo info{path::path_info(t_file_path)};

    if (!algo::any_equal(info, PathInfo::file, PathInfo::new_file))
    {
        t_error = path::make_error(INVALID_PATH_MSG, t_file_path);
    }
    else if (info == PathInfo::new_file && path::readonly_permitted(t_mode))
    {
        t_error = path::make_error(FILE_NOT_FOUND_MSG, t_file_path);
    }
    else  // Open underlying file stream
    {
        m_path = path::resolve(t_file_path.string());
        m_mode = t_mode;
        m_fstream.open(m_path, m_mode);

        // Update file system error reference
        if (fail() || !is_open())
        {
            t_error = path::make_error(FILE_OPEN_FAILED_MSG, t_file_path);
            close();
        }
        else  // Reset error reference
        {
            path::reset_error(t_error);
        }
    }
}

/**
* @brief
*     Write the given data to the underlying file stream. Line-endings in
*     the data will be normalized using the underlying EOL control sequence.
*/
inline void scan::File::write(const LShift auto& t_data)
{
    filesystem_error error{path::make_error()};
    write(t_data, error);

    if (path::is_error(error))
    {
        throw RuntimeEx{error.what(), "File::write"};
    }
}

/**
* @brief
*     Write the given data to the underlying file stream. Line-endings in
*     the data will be normalized using the underlying EOL control sequence.
*/
inline void scan::File::write(const LShift auto& t_data, filesystem_error& t_error)
{
    if (!is_open())
    {
        t_error = path::make_error(FILE_CLOSED_MSG);
    }
    else if (fail())
    {
        t_error = path::make_error(FILE_FAIL_STATE_MSG);
    }
    else if (!path::write_permitted(m_mode))
    {
        t_error = path::make_error(FILE_OP_UNPERMITTED_MSG);
    }
    else  // Write data to file stream
    {
        m_fstream << algo::normalize_eol(t_data, m_eol);
    }
}

#endif // !SCAN_FILE_H
