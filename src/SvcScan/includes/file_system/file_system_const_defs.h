/*
* @file
*     file_system_const_defs.h
* @brief
*     Header file for file system constant expression fields.
*/
#pragma once

#ifndef SCAN_FILE_SYSTEM_CONST_DEFS_H
#define SCAN_FILE_SYSTEM_CONST_DEFS_H

#include <ios>
#include "../utils/aliases.h"
#include "file_system_aliases.h"

namespace scan
{
    /// @brief  Open mode bitmask permitting read operations.
    constexpr open_mode_t READ_BITMASK = ios_base::in;

    /// @brief  Open mode bitmask permitting write operations.
    constexpr open_mode_t WRITE_BITMASK = ios_base::out | ios_base::app | ios_base::trunc;

    /// @brief  User home file path alias.
    constexpr c_string_t HOME_ALIAS = "~";

    /// @brief  Standard file path delimiter.
    constexpr c_string_t PATH_DELIM = "/";

    /// @brief  Alternative file path delimiter.
    constexpr c_string_t PATH_DELIM_ALT = "\\";

    /// @brief  User profile environment variable name.
    constexpr c_string_t USER_PROFILE = "UserProfile";
}

#endif // !SCAN_FILE_SYSTEM_CONST_DEFS_H
