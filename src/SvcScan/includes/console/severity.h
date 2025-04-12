/*
* @file
*     severity.h
* @brief
*     Header file for a console output severity level enumeration.
*/
#pragma once

#ifndef SCAN_SEVERITY_H
#define SCAN_SEVERITY_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     Console output severity level enumeration.
    */
    enum class Severity : uint8_t
    {
        info,    // Informational severity level
        error,   // Error severity level
        warn,    // Warning severity level
        success  // Success severity level
    };
}

#endif // SCAN_SEVERITY_H
