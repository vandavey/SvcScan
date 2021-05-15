/*
*  hoststate.h
*  -----------
*  Header file for destination endpoint host state
*/
#pragma once

#ifndef HOST_STATE_H
#define HOST_STATE_H

namespace scan
{
    /// ***
    /// Destination endpoint host state enumeration type
    /// ***
    enum class HostState : short
    {
        unknown,  // Host down or unresponsive
        closed,   // Host rejecting connections
        open      // Host accepting connections
    };
}

#endif // !HOST_STATE_H
