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
#include "../concepts/concepts.h"
#include "type_defs.h"

namespace scan
{
    /// @brief  Byte size (in bits).
    constexpr uint8_t BYTE_LEN = 8U;

    /// @brief  Maximum byte value.
    constexpr uint8_t BYTE_MAX = 0xFF;

    /// @brief  Null (minimum) byte value.
    constexpr uint8_t BYTE_NULL = 0x00;

    /// @brief  Null character value.
    constexpr char CHAR_NULL = '\0';

    /// @brief  Success return code.
    constexpr int RCODE_NO_ERROR = 0;

    /// @brief  Maximum word value.
    constexpr uint16_t WORD_MAX = 0xFFFF;

    /// @brief  Null (minimum) word value.
    constexpr uint16_t WORD_NULL = 0x0000;

#ifdef _WIN64
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 1099511628211;
#else
    /// @brief  FNV-1a hash function offset basis value.
    constexpr size_t FNV_OFFSET_BASIS = 2166136261;

    /// @brief  FNV-1a hash function prime value.
    constexpr size_t FNV_PRIME = 16777619;
#endif // _WIN64

    /// @brief  Invalid stream size.
    constexpr streamsize INVALID_SIZE = -1;

    /// @brief  Application name.
    constexpr cstr_t APP = "SvcScan";

    /// @brief  Carriage-return control sequence.
    constexpr cstr_t CR = "\r";

    /// @brief  Line-feed control sequence.
    constexpr cstr_t LF = "\n";

    /// @brief  Carriage-return line-feed control sequence.
    constexpr cstr_t CRLF = "\r\n";

    /// @brief  Application repository URL.
    constexpr cstr_t REPO = "https://github.com/vandavey/SvcScan";

    /**
    * @brief
    *     Hash a byte array using hash algorithm FNV-1a.
    */
    template<HashableByte T>
    constexpr size_t fnv_1a_hash(const T *t_bytes_ptr, const size_t &t_count) noexcept
    {
        size_t hash{ FNV_OFFSET_BASIS };

        for (size_t i{ 0 }; i < sizeof(uchar_t) * t_count; i++)
        {
            hash = (hash ^ static_cast<size_t>(t_bytes_ptr[i])) * FNV_PRIME;
        }
        return hash;
    }

    /**
    * @brief
    *     Hash a byte using hash algorithm FNV-1a.
    */
    template<HashableByte T>
    constexpr size_t fnv_1a_hash(const T &t_byte) noexcept
    {
        const uchar_t uchar{ static_cast<uchar_t>(t_byte) };
        return fnv_1a_hash(&uchar, 1);
    }
}

#endif // !SCAN_EXPR_H
