/*
* @file
*     file.h
* @brief
*     Header file for a system file stream.
*/
#pragma once

#ifndef SCAN_FILE_H
#define SCAN_FILE_H

#include <fstream>
#include <string>
#include <utility>
#include "../concepts/concepts.h"
#include "../errors/error_const_defs.h"
#include "../errors/logic_ex.h"
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "../utils/eol.h"
#include "file_system_aliases.h"

namespace scan
{
    /**
    * @brief
    *     System file stream.
    */
    class File
    {
    private:  /* Fields */
        Eol m_eol;          // EOL control sequence
        openmode m_mode;    // File open mode

        string m_path;      // File path
        fstream m_fstream;  // File stream

    public:  /* Constructors & Destructor */
        File() noexcept;
        File(const File&) = delete;
        File(File&& t_file) noexcept;
        File(const string& t_path, openmode t_mode = default_mode(), Eol t_eol = Eol::lf);

        virtual ~File() = default;

    public:  /* Operators */
        File& operator=(const File&) = default;

        /**
        * @brief
        *     Move assignment operator overload.
        */
        constexpr File& operator=(File&& t_file) noexcept
        {
            if (this != &t_file)
            {
                m_eol = t_file.m_eol;
                m_fstream = std::move(t_file.m_fstream);
                m_mode = t_file.m_mode;
                m_path = std::move(t_file.m_path);
            }
            return *this;
        }

        File& operator<<(const LShift auto& t_data);

    public:  /* Methods */
        static void write(const string& t_path,
                          const LShift auto& t_data,
                          Eol t_eol = Eol::lf);

        static string read(const string& t_path, Eol t_eol = Eol::lf);

        void close();
        void open();
        void open(const string& t_path, openmode t_mode);
        void write(const LShift auto& t_data);

        bool is_open() const noexcept;

        string read();

    private:  /* Methods */
        /**
        * @brief
        *     Determine whether the given open mode permits file read operations.
        */
        static constexpr bool read_permitted(openmode t_mode) noexcept
        {
            return (t_mode & ios_base::in) != 0;
        }

        /**
        * @brief
        *     Determine whether the given open mode only permits file read operations.
        */
        static constexpr bool read_only_permitted(openmode t_mode) noexcept
        {
            return read_permitted(t_mode) && !write_permitted(t_mode);
        }

        /**
        * @brief
        *     Determine whether the given open mode permits file write operations.
        */
        static constexpr bool write_permitted(openmode t_mode) noexcept
        {
            return (t_mode & (ios_base::out | ios_base::app | ios_base::trunc)) != 0;
        }

        /**
        * @brief
        *     Get the default file stream open mode for read and write operations.
        */
        static constexpr openmode default_mode() noexcept
        {
            return (default_read_mode() | default_write_mode()) & ~ios_base::trunc;
        }

        /**
        * @brief
        *     Get the default file stream open mode for read operations.
        */
        static constexpr openmode default_read_mode() noexcept
        {
            return ios_base::binary | ios_base::in;
        }

        /**
        * @brief
        *     Get the default file stream open mode for write operations.
        */
        static constexpr openmode default_write_mode() noexcept
        {
            return ios_base::binary | ios_base::out | ios_base::trunc;
        }
    };
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
inline void scan::File::write(const string& t_path, const LShift auto& t_data, Eol t_eol)
{
    File file{t_path, default_write_mode(), t_eol};

    file.write(t_data);
    file.close();
}

/**
* @brief
*     Write the given data to the underlying file stream. Line-endings in
*     the data will be normalized using the underlying EOL control sequence.
*/
inline void scan::File::write(const LShift auto& t_data)
{
    if (!is_open())
    {
        throw LogicEx{FILE_CLOSED_MSG, "File::write"};
    }

    if (!write_permitted(m_mode))
    {
        throw LogicEx{FILE_OP_UNPERMITTED_MSG, "File::write"};
    }
    m_fstream << algo::normalize_eol(t_data, m_eol);
}

#endif // !SCAN_FILE_H
