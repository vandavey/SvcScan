/*
*  filestream.h
*  ------------
*  Header file for handling file stream operations and data
*/
#pragma once

#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <fstream>
#include <string>
#include "../except/logicex.h"
#include "../io/stdutil.h"
#include "../rc/textrc.h"
#include "path.h"

namespace scan
{
    /// ***
    /// File stream wrapper and data controller
    /// ***
    class FileStream
    {
    public:  /* Type Aliases */
        using fstream = std::fstream;

    private:  /* Type Aliases */
        using filebuf    = std::filebuf;
        using fspath     = Path::fspath;
        using openmode   = fstream::openmode;
        using stdu       = StdUtil;
        using sstream    = std::stringstream;
        using streamsize = std::streamsize;
        using string     = std::string;
        using vector_s   = std::vector<string>;

    private:  /* Constants */
        static constexpr streamsize INVALID_SIZE{ -1I64 };

    public:  /* Fields */
        openmode mode; // File open mode
        string path;   // File path

    private:  /* Fields */
        fstream m_file;  // File stream

    public:  /* Constructors & Destructor */
        FileStream();
        FileStream(const string &t_path, const openmode &t_mode);

        virtual ~FileStream() = default;

    private:  /* Constructors (deleted) */
        FileStream(const FileStream &) = delete;

    public:  /* Operators */
        std::istream &operator>>(string &t_buffer);

        template<class T>
        FileStream &operator<<(const T &t_data);

    public:  /* Methods */
        static string read_text(const string &t_path);

        static vector_s read_lines(const string &t_path);

        void close();
        void open(const openmode &t_mode);

        template<class T>
        void write(const T &t_data, const bool &t_close = false);

        bool is_open() const;

        streamsize size(const bool &t_close = false);

        string read_text(const bool &t_close = false);
        vector_s read_lines(const bool &t_close = false);

    private:  /* Methods */
        static bool valid_mode(const openmode &t_mode);
    };
}

/// ***
/// Bitwise left shift operator overload
/// ***
template<class T>
inline scan::FileStream &scan::FileStream::operator<<(const T &t_data)
{
    if (!m_file.is_open())
    {
        throw LogicEx{ "FileStream::operator<<", "Underlying file closed" };
    }
    write(t_data);
    return *this;
}

/// ***
/// Write data to the underlying file stream
/// ***
template<class T>
inline void scan::FileStream::write(const T &t_data, const bool &t_close)
{
    if (!m_file.is_open())
    {
        throw LogicEx{ "FileStream::write", "Underlying file closed" };
    }
    m_file << t_data;

    if (t_close)
    {
        m_file.close();
    }
}

#endif // !FILE_STREAM_H
