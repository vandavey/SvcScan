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

namespace
{
    namespace filesystem = std::filesystem;
}

namespace scan
{
    /// ***
    /// File path and file system utilities
    /// ***
    class Path
    {
    private:  /* Types */
        using path = filesystem::path;

        using string   = std::string;
        using vector_s = std::vector<string>;

    public:  /* Destructor */
        virtual ~Path() = default;

    private:  /* Constructors (deleted) */
        Path() = delete;
        Path(const Path &) = delete;

    public:  /* Methods */
        static bool create_file(const string &t_spath);

        static bool exists(const string &t_spath);
        static bool is_dir(const string &t_spath);
        static bool is_valid(const string &t_spath);

        static string parent(const string &t_spath);
        static string resolve(const string &t_spath);

    private: /* Methods */
        static bool ends_with(const string &t_spath, const string &t_sub);
        static bool ends_with(const string &t_spath, const vector_s &t_svect);
    };
}

#endif // !PATH_H
