/*
* @file
*     color.h
* @brief
*     Header file for a console color enumeration type.
*/
#pragma once

#ifndef SCAN_COLOR_H
#define SCAN_COLOR_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     Console color enumeration type.
    */
    enum class Color : uint8_t
    {
        normal,  // Default console color
        cyan,    // Cyan console color
        green,   // Green console color
        red,     // Red console color
        yellow   // Yellow console color
    };
}

#endif // !SCAN_COLOR_H
