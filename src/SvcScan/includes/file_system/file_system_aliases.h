/*
* @file
*     file_system_aliases.h
* @brief
*     Header file for file system type aliases.
*/
#pragma once

#ifndef SCAN_FILE_SYSTEM_ALIASES_H
#define SCAN_FILE_SYSTEM_ALIASES_H

#include <filesystem>
#include <ios>
#include <iosfwd>

namespace scan
{
    namespace filesystem = std::filesystem;

    using file_path_t = filesystem::path;
    using file_type   = filesystem::file_type;
    using fstream     = std::fstream;
    using ios_base    = std::ios_base;
    using openmode    = ios_base::openmode;
}

#endif // !SCAN_FILE_SYSTEM_ALIASES_H
