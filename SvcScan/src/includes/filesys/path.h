/*
*  path.h
*  ------
*  Header file for file system and path utilities
*/
#pragma once

#ifndef PATH_H
#define PATH_H

#include <filesystem>
#include <string>
#include "pathinfo.h"

namespace
{
    namespace fs = std::filesystem;
}

namespace scan
{
    /// ***
    /// File path and file system utilities
    /// ***
    class Path
    {
    public:  /* Types */
        using fspath = fs::path;

    private:  /* Types */
        using string   = std::string;
        using vector_s = std::vector<string>;

    public:  /* Destructor */
        virtual ~Path() = default;

    private:  /* Constructors (deleted) */
        Path() = delete;
        Path(const Path &) = delete;

    public:  /* Methods */
        //static bool create_file(const string &t_path);
        static bool exists(const string &t_path);
        static bool is_absolute(const string &t_path);
        static bool is_directory(const string &t_path);
        static bool valid_file(const string &t_path);

        static PathInfo path_info(const string &t_path);

        static string parent(const string &t_path);
        static string resolve(const string &t_path);

    private: /* Methods */
        static bool ends_with(const string &t_path, const string &t_sub);
        static bool ends_with(const string &t_path, const vector_s &t_svect);
    };
}

#endif // !PATH_H
