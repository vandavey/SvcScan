/*
* @file
*     svc_field.h
* @brief
*     Header file for a table field enumeration type.
*/
#pragma once

#ifndef SCAN_SVC_FIELD_H
#define SCAN_SVC_FIELD_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     Enumeration type for a 'SvcTable' field.
    */
    enum class SvcField : uint8_t
    {
        port,     // Port number
        service,  // Service name
        state,    // Port state
        summary   // Service summary
    };
}

#endif // !SCAN_SVC_FIELD_H
