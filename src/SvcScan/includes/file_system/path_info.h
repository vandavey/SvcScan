/*
* @file
*     path_info.h
* @brief
*     Header file for a file path information enumeration.
*/
#pragma once

#ifndef SCAN_PATH_INFO_H
#define SCAN_PATH_INFO_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     File path information enumeration.
    */
    enum class PathInfo : uint8_t
    {
        unknown,        // Unknown or unsupported path type
        empty,          // Empty file or directory path
        file,           // Existing file path
        directory,      // Existing directory path
        new_file,       // New file path (with existing parent path)
        new_directory,  // New directory path (with existing parent path)
        not_found       // Nonexistant file or directory path
    };
}

#endif // !SCAN_PATH_INFO_H
