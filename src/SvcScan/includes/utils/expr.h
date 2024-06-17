/*
* @file
*     expr.h
* @brief
*     Header file for common constant expressions.
*/
#pragma once

#ifndef SCAN_EXPR_H
#define SCAN_EXPR_H

#include <cstdint>
#include "type_defs.h"

namespace scan
{
    /// @brief  Dash character value.
    constexpr char CHAR_DASH = '-';

    /// @brief  Null character value.
    constexpr char CHAR_NULL = '\0';

    /// @brief  Success return code.
    constexpr int RCODE_NO_ERROR = 0;

    /// @brief  Maximum word value.
    constexpr uint16_t UINT16_T_MAX = 0xFFFFU;

    /// @brief  Null (minimum) word value.
    constexpr uint16_t UINT16_T_NULL = 0x0000U;

#ifdef _WIN64
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 0XCBF29CE484222325ULL;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 0x100000001B3ULL;
#elif _WIN32 // _WIN64
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 0X811C9DC5U;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 0x1000193U;
#endif // _WIN32

    /// @brief  Invalid stream size.
    constexpr streamsize INVALID_SIZE = -1LL;

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

#endif // !SCAN_EXPR_H
