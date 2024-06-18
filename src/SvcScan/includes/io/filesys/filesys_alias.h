/*
* @file
*     filesys_alias.h
* @brief
*     Header file for file system type aliases.
*/
#pragma once

#ifndef SCAN_FILESYS_ALIAS_H
#define SCAN_FILESYS_ALIAS_H

#include <filesystem>
#include <ios>
#include <iosfwd>

namespace scan
{
    namespace filesystem = std::filesystem;

    using filebuf  = std::filebuf;
    using fstream  = std::fstream;
    using ios_base = std::ios_base;
    using openmode = ios_base::openmode;
    using path_t   = filesystem::path;
}

#endif // !SCAN_FILESYS_ALIAS_H
