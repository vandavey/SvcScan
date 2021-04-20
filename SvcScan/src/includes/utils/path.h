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

namespace scan
{
    namespace
    {
        namespace filesystem = std::filesystem;
    }

    /// ***
    /// String and standard stream manipulation utilities
    /// ***
    class Path
    {
    private:  /* Types */
        using string = std::string;
        using vector_s = std::vector<string>;

        using path = filesystem::path;

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
    };
}

#endif // !PATH_H
