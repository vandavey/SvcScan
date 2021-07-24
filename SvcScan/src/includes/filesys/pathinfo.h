/*
*  pathinfo.h
*  ----------
*  Header file for file path information enumeration type
*/
#pragma once

#ifndef PATH_INFO_H
#define PATH_INFO_H

namespace scan
{
    /// ***
    /// Output file path information enumeration type
    /// ***
    enum class PathInfo : short
    {
        exists,            // Existing file path
        directory,         // File path is a directory
        empty,             // File path is empty
        parent_exists,     // Parent path exists (file does not exists)
        parent_not_found,  // Parent directory path does not exists
    };
}

#endif // !PATH_INFO_H
