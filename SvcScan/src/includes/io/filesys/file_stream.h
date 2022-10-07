/*
*  file_stream.h
*  -------------
*  Header file for a system file stream
*/
#pragma once

#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <fstream>
#include "../../except/logic_ex.h"
#include "path.h"

namespace scan
{
    /**
    * @brief  System file stream wrapper.
    */
    class FileStream
    {
    private:  /* Type Aliases */
        using this_t = FileStream;

        using uint = unsigned int;

        using filebuf    = std::filebuf;
        using fspath     = Path::fspath;
        using fstream    = std::fstream;
        using openmode   = fstream::openmode;
        using stdu       = StdUtil;
        using sstream    = std::stringstream;
        using streamsize = std::streamsize;
        using string     = std::string;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Constants */
        static constexpr streamsize INVALID_SIZE{ -1i64 };  // Invalid stream size

    public:  /* Fields */
        openmode mode; // File open mode
        string path;   // File path

    private:  /* Fields */
        fstream m_file;  // File stream

    public:  /* Constructors & Destructor */
        FileStream();
        FileStream(const FileStream &) = delete;
        FileStream(FileStream &&t_fstream) noexcept;
        FileStream(const string &t_path, const openmode &t_mode = write_mode(true));

        virtual ~FileStream();

    public:  /* Operators */
        FileStream &operator=(const FileStream &) = default;
        FileStream &operator=(FileStream &&t_fstream) noexcept;

        std::istream &operator>>(string &t_buffer);

        template<LShift T>
        FileStream &operator<<(const T &t_data);

    public:  /* Methods */
        static void write(const string &t_path,
                          const string &t_data,
                          const bool &t_binary = true);

        static openmode read_mode(const bool &t_binary = true) noexcept;
        static openmode write_mode(const bool &t_binary = true) noexcept;

        static string read(const string &t_path, const bool &t_binary = true);

        void close();
        void open();
        void open(const string &t_path, const openmode &t_mode = write_mode());

        template<LShift T>
        void write(const T &t_data, const bool &t_close = false);

        bool is_open() const;

        streamsize size(const bool &t_close = false);

        string read(const bool &t_close = false);

    private:  /* Methods */
        void throw_if_failed() const;
    };
}

/**
* @brief  Bitwise left shift operator overload.
*/
template<scan::LShift T>
inline scan::FileStream &scan::FileStream::operator<<(const T &t_data)
{
    write(t_data);
    return *this;
}

/**
* @brief  Write all the given data to the underlying file stream and optionally
*         close the underlying file stream.
*/
template<scan::LShift T>
inline void scan::FileStream::write(const T &t_data, const bool &t_close)
{
    if (!m_file.is_open())
    {
        throw LogicEx{ "FileStream::write", "Underlying file is closed" };
    }

    m_file << t_data;
    throw_if_failed();

    if (t_close)
    {
        m_file.close();
        throw_if_failed();
    }
}

#endif // !FILE_STREAM_H
