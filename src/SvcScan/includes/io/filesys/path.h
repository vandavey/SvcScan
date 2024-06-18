/*
* @file
*     path.h
* @brief
*     Header file for file system and path utilities.
*/
#pragma once

#ifndef SCAN_PATH_H
#define SCAN_PATH_H

#include "../../utils/alias.h"
#include "path_info.h"

namespace scan
{
    /**
    * @brief
    *     File path and file system utilities.
    */
    class Path final
    {
    public:  /* Constructors & Destructor */
        Path() = delete;
        Path(const Path &) = delete;
        Path(Path &&) = delete;

        virtual ~Path() = default;

    public:  /* Operators */
        Path &operator=(const Path &) = default;
        Path &operator=(Path &&) = default;

    public:  /* Methods */
        static bool exists(const string &t_path);
        static bool is_absolute(const string &t_path);
        static bool is_directory(const string &t_path);
        static bool valid_file(const string &t_path);

        static PathInfo path_info(const string &t_path);

        static string parent(const string &t_path);
        static string resolve(const string &t_path);

        static string_vector parts(const string &t_path);

    private:  /* Methods */
        static string normalize(const string &t_path);
        static string user_home(const string &t_env_var = "USERPROFILE");
    };
}

#endif // !SCAN_PATH_H
