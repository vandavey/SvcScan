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
    /// ***
    /// Enumeration type for a 'SvcInfo' object field
    /// ***
    enum class SvcField : short
    {
        port,     // Port number
        service,  // Service name
        state,    // Port state
        info      // Service information
    };
}

#endif // !SVC_FIELD_H
