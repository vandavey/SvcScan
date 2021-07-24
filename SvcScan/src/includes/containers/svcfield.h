/*
*  svcfield.h
*  ----------
*  Header file for SvcInfo table field
*/
#pragma once

#ifndef SVC_FIELD_H
#define SVC_FIELD_H

namespace scan
{
    /// ***
    /// Table field enumeration type for SvcInfo field
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
