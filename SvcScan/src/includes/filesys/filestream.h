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
    private:  /* Types */
        using fspath   = Path::fspath;
        using ofs      = std::ofstream;
        using openmode = ofs::openmode;
        using string   = std::string;

    public:  /* Fields */
        AutoProp<openmode> mode; // File open mode
        AutoProp<string> path;   // Output file path

    private:  /* Fields */
        ofs m_file;  // Output file stream

    public:  /* Constructors & Destructor */
        FileStream() = default;

        FileStream(const string &t_path,
                   const openmode &t_mode = ofs::out | ofs::trunc);

        virtual ~FileStream() = default;

    public:  /* Constructors (deleted) */
        FileStream(const FileStream &) = delete;

    public:  /* Operators */
        template<class T>
        FileStream &operator<<(const T &t_data);

    public:  /* Methods */
        template<class T>
        static void write(const string &t_path,
                          const T &t_data,
                          const openmode &t_mode = ofs::out | ofs::trunc);

        void close();
        void open(const openmode &t_mode = ofs::out | ofs::trunc);

        template<class T>
        void write(const T &t_data, const bool &t_close = false);

        bool is_open() const;

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
        throw LogicEx("FileStream::operator<<", "Underlying file must be opened");
    }
    write(t_data);
    return *this;
}

/// ***
/// Utility - Write given data to the specified file path
/// ***
template<class T>
inline void scan::FileStream::write(const string &t_path,
                                    const T &t_data,
                                    const openmode &t_mode) {
    FileStream fs{ t_path, t_mode };
    fs << t_data << StdUtil::LF;

    fs.close();
}

/// ***
/// Write data to the underlying file stream
/// ***
template<class T>
inline void scan::FileStream::write(const T &t_data, const bool &t_close)
{
    if (!m_file.is_open())
    {
        throw LogicEx("FileStream::write", "Underlying file must be opened");
    }

    // Insert data into stream
    m_file << t_data;

    if (t_close)
    {
        m_file.close();
    }
}

#endif // !FILE_STREAM_H
