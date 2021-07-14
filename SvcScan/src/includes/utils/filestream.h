/*
*  streamer.h
*  ----------
*  Header file for handling file stream data and operations
*/
#pragma once

#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <fstream>
#include <string>
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
        using string   = std::string;
        using ofstream = std::ofstream;

        using fspath   = std::filesystem::path;
        using openmode = ofstream::openmode;

    public:  /* Fields */
        static AutoProp<string> default_path;  // Default output path

        AutoProp<openmode> mode;               // File open mode
        AutoProp<string> path;                 // Output file path

    private:  /* Fields */
        std::fstream m_file;  // Output file stream

    public:  /* Constructors & Destructor */
        FileStream();

        FileStream(const string &t_path,
                   const openmode &t_mode = ofstream::out | ofstream::trunc);

        virtual ~FileStream() = default;

    public:  /* Constructors (deleted) */
        FileStream(const FileStream &) = delete;

    public:  /* Operators */
        template<class T>
        FileStream &operator<<(const T &t_data);

    public:  /* Methods */
        void close();

        void open(const string &t_path = string(),
                  const openmode &t_mode = ofstream::app);

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
