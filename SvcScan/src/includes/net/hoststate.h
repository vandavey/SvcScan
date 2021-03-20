/*
*  hoststate.h
*  -----------
*  Header file for destination endpoint host state
*/
#pragma once

#ifndef HOST_STATE_H
#define HOST_STATE_H

namespace Scan
{
    /// ***
    /// Destination endpoint host state enumeration type
    /// ***
    enum class HostState : short
    {
        unknown,  // Destination down or not responding
        closed,   // Destination not accepting connections
        open      // Destination accepting connections
    };
}

#endif // !HOST_STATE_H
