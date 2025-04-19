/*
* @file
*     svc_field.h
* @brief
*     Header file for a network application service table field enumeration.
*/
#pragma once

#ifndef SCAN_SVC_FIELD_H
#define SCAN_SVC_FIELD_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     Network application service table field enumeration.
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
