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

    /// @brief  Maximum word value.
    constexpr uint16_t UINT16_T_MAX = 0xffff_u16;

    /// @brief  Null (minimum) word value.
    constexpr uint16_t UINT16_T_NULL = 0x0000_u16;

    /// @brief  Error return code.
    constexpr int RCODE_ERROR = 1;

    /// @brief  Success return code.
    constexpr int RCODE_NO_ERROR = 0;

#ifdef _WIN64
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 0xcbf29ce484222325_sz;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 0x100000001b3_sz;
#elif _WIN32 // _WIN64
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 0x811c9dc5_sz;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 0x1000193_sz;
#endif // _WIN32

    /// @brief  Default wrapped line size.
    constexpr size_t LN_SIZE_DEFAULT = 95_sz;

    /// @brief  Minimum wrapped line size.
    constexpr size_t LN_SIZE_MIN = 50_sz;

    /// @brief  Maximum size type value.
    constexpr size_t NPOS = (std::numeric_limits<size_t>::max)();

    /// @brief  String decimal point precision.
    constexpr streamsize PRECISION = 4_i64;

    /// @brief  Application name.
    constexpr c_string_t APP = "SvcScan";

    /// @brief  Carriage-return control sequence.
    constexpr c_string_t CR = "\r";

    /// @brief  ANSI control sequence introducer.
    constexpr c_string_t CSI = "\x1b[";

    /// @brief  Line-feed control sequence.
    constexpr c_string_t LF = "\n";

    /// @brief  Carriage-return line-feed control sequence.
    constexpr c_string_t CRLF = "\r\n";

    /// @brief  Modulus value.
    constexpr c_string_t MOD = "%";

    /// @brief  Application repository URL.
    constexpr c_string_t REPO = "https://github.com/vandavey/SvcScan";

    /// @brief  String trimming characters.
    constexpr c_string_t TRIM_CHARS = "\t\n\v\f\r ";

    /// @brief  String wrapping delimiter characters.
    constexpr c_string_t WRAP_CHARS = "\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~";
}

#endif // !SCAN_CONST_DEFS_H
