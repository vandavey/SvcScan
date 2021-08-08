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
#include "../conio/stdutil.h"
#include "../except/logicex.h"
#include "../properties/autoprop.h"
#include "path.h"

namespace scan
{
    /// ***
    /// File stream wrapper and data controller
    /// ***
    class FileStream
    {
    public:  /* Types */
        using fstream = std::fstream;

    private:  /* Types */
        using fspath   = Path::fspath;
        using openmode = fstream::openmode;
        using stdu     = StdUtil;
        using string   = std::string;
        using vector_s = std::vector<string>;

    public:  /* Fields */
        AutoProp<openmode> mode; // File open mode
        AutoProp<string> path;   // Output file path

    private:  /* Fields */
        fstream m_file;  // File stream

    public:  /* Constructors & Destructor */
        FileStream() = default;
        FileStream(const string &t_path, const openmode &t_mode);

        virtual ~FileStream() = default;

    private:  /* Constructors (deleted) */
        FileStream(const FileStream &) = delete;

    public:  /* Operators */
        std::istream &operator>>(string &t_buffer);

        template<class T>
        FileStream &operator<<(const T &t_data);

    public:  /* Methods */
        static string read_csv(const string &t_path);
        static vector_s read_csv_lines(const string &t_path);

        void close();
        void open(const openmode &t_mode);

        template<class T>
        void write(const T &t_data, const bool &t_close = false);

        bool is_open() const;

        string read_csv(const bool &t_close = false);
        vector_s read_csv_lines(const bool &t_close = false);

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
        throw LogicEx("FileStream::operator<<", "Underlying file must be open");
    }
    write(t_data);
    return *this;}

/// ***
/// Write data to the underlying file stream
/// ***
template<class T>
inline void scan::FileStream::write(const T &t_data, const bool &t_close)
{
    if (!m_file.is_open())
    {
        throw LogicEx("FileStream::write", "Underlying file must be open");
    }
    m_file << t_data;

    if (t_close)
    {
        m_file.close();
    }
}

#endif // !FILE_STREAM_H
