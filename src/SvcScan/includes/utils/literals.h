/*
* @file
*     literals.h
* @brief
*     Header file for user-defined literals.
*/
#pragma once

#ifndef SCAN_LITERALS_H
#define SCAN_LITERALS_H

#include <chrono>
#include <cstdint>
#include "aliases.h"

namespace scan
{
    /**
    * @brief
    *     16-bit unsigned integer user-defined literal.
    */
    constexpr uint16_t operator""_u16(uint64_t t_num) noexcept
    {
        return static_cast<uint16_t>(t_num);
    }

    /**
    * @brief
    *     16-bit integer user-defined literal.
    */
    constexpr int16_t operator""_i16(uint64_t t_num) noexcept
    {
        return static_cast<int16_t>(t_num);
    }

    /**
    * @brief
    *     Size type integer user-defined literal.
    */
    constexpr size_t operator""_st(uint64_t t_num) noexcept
    {
        return static_cast<size_t>(t_num);
    }

    /**
    * @brief
    *     64-bit unsigned integer user-defined literal.
    */
    constexpr uint64_t operator""_u64(uint64_t t_num) noexcept
    {
        return t_num;
    }

    /**
    * @brief
    *     64-bit integer user-defined literal.
    */
    constexpr int64_t operator""_i64(uint64_t t_num) noexcept
    {
        return static_cast<int64_t>(t_num);
    }

    /**
    * @brief
    *     Milliseconds integer user-defined literal.
    */
    constexpr milliseconds operator""_ms(uint64_t t_ms) noexcept
    {
        return std::chrono_literals::operator""ms(t_ms);
    }
}

#endif // !SCAN_LITERALS_H
