/*
*  filesys_defs.h
*  --------------
*  Header file for file system type aliases and constants
*/
#pragma once

#ifndef FILESYS_DEFS_H
#define FILESYS_DEFS_H

#include <filesystem>
#include <fstream>
#include "../../utils/type_defs.h"

namespace scan
{
    namespace filesystem = std::filesystem;

    using filebuf  = std::filebuf;
    using fstream  = std::fstream;
    using ios_base = std::ios_base;
    using openmode = ios_base::openmode;
    using path_t   = filesystem::path;

    constexpr streamsize INVALID_SIZE = -1;  // Invalid stream size
}

#endif // !FILESYS_DEFS_H
