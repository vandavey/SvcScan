/*
*  svc_field.h
*  -----------
*  Header file for a table field enumeration type
*/
#pragma once

#ifndef SVC_FIELD_H
#define SVC_FIELD_H

#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Enumeration type for a 'SvcTable' field.
    */
    enum class SvcField : byte_t
    {
        port,     // Port number
        proto,    // Transport protocol
        service,  // Service name
        state,    // Port state
        summary   // Service summary
    };
}

#endif // !SVC_FIELD_H
