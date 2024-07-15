/*
* @file
*     file_stream.h
* @brief
*     Header file for a system file stream.
*/
#pragma once

#ifndef SCAN_FILE_STREAM_H
#define SCAN_FILE_STREAM_H

#include <fstream>
#include <type_traits>
#include "../../concepts/concepts.h"
#include "../../errors/logic_ex.h"
#include "../../utils/aliases.h"
#include "filesys_aliases.h"

namespace scan
{
    /**
    * @brief
    *     System file stream wrapper.
    */
    class FileStream
    {
    private:  /* Fields */
        openmode m_mode;  // File open mode
        string m_path;    // File path

        fstream m_file;   // File stream

    public:  /* Constructors & Destructor */
        FileStream() noexcept;
        FileStream(const FileStream &) = delete;
        FileStream(FileStream &&t_fstream) noexcept;
        FileStream(const string &t_path, const openmode &t_mode);

        virtual ~FileStream() = default;

    public:  /* Operators */
        FileStream &operator=(const FileStream &) = default;

        /**
        * @brief
        *     Move assignment operator overload.
        */
        constexpr FileStream &operator=(FileStream &&t_fstream) noexcept
        {
            if (this != &t_fstream)
            {
                m_file = std::move(t_fstream.m_file);
                m_mode = t_fstream.m_mode;
                m_path = t_fstream.m_path;
            }
            return *this;
        }

        istream &operator>>(string &t_buffer);

        template<LShift T>
        FileStream &operator<<(const T &t_data);

    public:  /* Methods */
        static void write(const string &t_path, const string &t_data);

        static string read(const string &t_path);

        void close();
        void open();
        void open(const string &t_path, const openmode &t_mode);

        template<LShift T>
        void write(const T &t_data);

        bool is_open() const noexcept;

        streamsize size();

        string read();

    private:  /* Methods */
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
inline scan::FileStream &scan::FileStream::operator<<(const T &t_data)
{
    write(t_data);
    return *this;
}

/**
* @brief
*     Write all the given data to the underlying file stream
*     and optionally close the underlying file stream.
*/
template<scan::LShift T>
inline void scan::FileStream::write(const T &t_data)
{
    if (!m_file.is_open())
    {
        throw LogicEx{ "FileStream::write", "Underlying file is closed" };
    }
    m_file << t_data;
}

#endif // !SCAN_FILE_STREAM_H
