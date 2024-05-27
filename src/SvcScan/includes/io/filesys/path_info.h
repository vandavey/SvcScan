/*
* @file
*     path_info.h
* @brief
*     Header file for a file path information enumeration type.
*/
#pragma once

#ifndef PATH_INFO_H
#define PATH_INFO_H

#include "../../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  File path information enumeration type.
    */
    enum class PathInfo : byte_t
    {
        unknown,          // Unknown file path information
        exists,           // Existing file path
        directory,        // File path is a directory
        empty,            // File path is empty
        parent_exists,    // Parent path exists (file does not exists)
        parent_not_found  // Nonexistent parent directory path
    };
}

#endif // !PATH_INFO_H
