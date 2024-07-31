/*
* @file
*     const_defs.h
* @brief
*     Header file for common constant expression fields.
*/
#pragma once

#ifndef SCAN_CONST_DEFS_H
#define SCAN_CONST_DEFS_H

#include <cstdint>
#include <limits>
#include "aliases.h"
#include "literals.h"

namespace scan
{
    /// @brief  Dash character value.
    constexpr char CHAR_DASH = '-';

    /// @brief  Null character value.
    constexpr char CHAR_NULL = '\0';

    /// @brief  Error return code.
    constexpr int RCODE_ERROR = 1;

    /// @brief  Success return code.
    constexpr int RCODE_NO_ERROR = 0;

    /// @brief  Maximum word value.
    constexpr uint16_t UINT16_T_MAX = 0xFFFF_u16;

    /// @brief  Null (minimum) word value.
    constexpr uint16_t UINT16_T_NULL = 0x0000_u16;

#ifdef _WIN64
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 0XCBF29CE484222325_st;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 0x100000001B3_st;
#elif _WIN32 // _WIN64
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 0X811C9DC5_st;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 0x1000193_st;
#endif // _WIN32

    /// @brief  Maximum size type value.
    constexpr size_t NPOS = (std::numeric_limits<size_t>::max)();

    /// @brief  Invalid stream size.
    constexpr streamsize INVALID_SIZE = -1_i64;

    /// @brief  Application name.
    constexpr cstr_t APP = "SvcScan";

    /// @brief  Carriage-return control sequence.
    constexpr cstr_t CR = "\r";

    /// @brief  Line-feed control sequence.
    constexpr cstr_t LF = "\n";

    /// @brief  Carriage-return line-feed control sequence.
    constexpr cstr_t CRLF = "\r\n";

    /// @brief  Modulus value.
    constexpr cstr_t MOD = "%";

    /// @brief  Application repository URL.
    constexpr cstr_t REPO = "https://github.com/vandavey/SvcScan";
}

#endif // !SCAN_CONST_DEFS_H
