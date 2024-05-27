/*
* @file
*     expr.h
* @brief
*     Header file for common constant expressions.
*/
#pragma once

#ifndef EXPR_H
#define EXPR_H

#include "../concepts/type_concepts.h"

namespace scan
{
    /// @brief  Byte size (in bits).
    constexpr byte_t BYTE_LEN = 8U;

    /// @brief  Maximum byte value.
    constexpr byte_t BYTE_MAX = 0xFF;

    /// @brief  Null (minimum) byte value.
    constexpr byte_t BYTE_NULL = 0x00;

    /// @brief  Null character value.
    constexpr char CHAR_NULL = '\0';

    /// @brief  Maximum word value.
    constexpr word_t WORD_MAX = 0xFFFF;

    /// @brief  Null (minimum) word value.
    constexpr word_t WORD_NULL = 0x0000;

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
    *     Hash an integral value using hash algorithm FNV-1a.
    */
    template<std::integral T>
    constexpr size_t fnv_1a_hash(const T *t_ptr, const size_t &t_len)
    {
        size_t hash{ FNV_OFFSET_BASIS };

        for (size_t i{ 0 }; i < t_len; i++)
        {
            hash = (hash ^ t_ptr[i]) * FNV_PRIME;
        }
        return hash;
    }

    /**
    * @brief
    *     Hash an integral value using hash algorithm FNV-1a.
    */
    template<std::integral T>
    constexpr size_t fnv_1a_hash(const T &t_value)
    {
        return fnv_1a_hash(&t_value, 1);
    }
}

#endif // !EXPR_H