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
#include <fstream>
#include <ios>

namespace scan
{
    namespace filesystem = std::filesystem;

    using open_mode_t = int;

    using file_type        = filesystem::file_type;
    using filesystem_error = filesystem::filesystem_error;
    using fstream          = std::fstream;
    using ios_base         = std::ios_base;
    using path_t           = filesystem::path;
}

#endif // !SCAN_FILE_SYSTEM_ALIASES_H
