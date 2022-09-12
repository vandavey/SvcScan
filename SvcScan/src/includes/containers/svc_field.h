/*
*  svc_field.h
*  -----------
*  Header file for a table field enumeration type
*/
#pragma once

#ifndef SVC_FIELD_H
#define SVC_FIELD_H

namespace scan
{
    /**
    * @brief  Enumeration type for a 'SvcTable' field.
    */
    enum class SvcField : unsigned int
    {
        port,     // Port number
        proto,    // Transport protocol
        service,  // Service name
        state,    // Port state
        info      // Service information
    };
}

#endif // !SVC_FIELD_H
