/*
* @file
*     eol.h
* @brief
*     Header file for a line-ending control sequence enumeration.
*/
#pragma once

#ifndef SCAN_EOL_H
#define SCAN_EOL_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     Line-ending control sequence enumeration.
    */
    enum class Eol : uint8_t
    {
        lf,   // Line-feed
        cr,   // Carriage-return
        crlf  // Carriage-return and line-feed
    };
}

#endif // !SCAN_EOL_H
