/*
* @file
*     file.h
* @brief
*     Header file for a system file stream.
*/
#pragma once

#ifndef SCAN_FILE_STREAM_H
#define SCAN_FILE_STREAM_H

#include <fstream>
#include <utility>
#include "../concepts/concepts.h"
#include "../errors/logic_ex.h"
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "../utils/eol.h"
#include "file_system_aliases.h"

namespace scan
{
    /**
    * @brief
    *     System file stream wrapper.
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

        template<LShift T>
        File& operator<<(const T& t_data);

    public:  /* Methods */
        template<LShift T>
        static void write(const string& t_path, const T& t_data, Eol t_eol = Eol::lf);

        static string read(const string& t_path, Eol t_eol = Eol::lf);

        void close();
        void open();
        void open(const string& t_path, openmode t_mode);

        template<LShift T>
        void write(const T& t_data);

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
        *     Determine whether the given open mode permits file write operations.
        */
        static constexpr bool write_permitted(openmode t_mode) noexcept
        {
            return (t_mode & (ios_base::out | ios_base::app | ios_base::trunc)) != 0;
        }

        /**
        * @brief
        *     Get the default file stream open mode for read or write operations.
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
template<scan::LShift T>
inline scan::File& scan::File::operator<<(const T& t_data)
{
    write(t_data);
    return *this;
}

/**
* @brief
*     Write the given data to the specified file path and close the stream. Line-endings
*     in the data will be normalized using the specified EOL control sequence.
*/
template<scan::LShift T>
inline void scan::File::write(const string& t_path, const T& t_data, Eol t_eol)
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
template<scan::LShift T>
inline void scan::File::write(const T& t_data)
{
    if (!is_open())
    {
        throw LogicEx{"File::write", "Underlying file is closed"};
    }

    if (!write_permitted(m_mode))
    {
        throw LogicEx{"File::write", "Underlying file does not permit write operations"};
    }
    m_fstream << algo::normalize_eol(t_data, m_eol);
}

#endif // !SCAN_FILE_STREAM_H
