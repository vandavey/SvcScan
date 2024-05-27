/*
* @file
*     color.h
* @brief
*     Header file for a console color enumeration type.
*/
#pragma once

#ifndef COLOR_H
#define COLOR_H

#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief
    *     Console color enumeration type.
    */
    enum class Color : byte_t
    {
        normal,  // Default console color
        cyan,    // Cyan console color
        green,   // Green console color
        red,     // Red console color
        yellow   // Yellow console color
    };
}

#endif // !COLOR_H
