/*
* @file
*     host_state.h
* @brief
*     Header file for destination endpoint host state.
*/
#pragma once

#ifndef HOST_STATE_H
#define HOST_STATE_H

#include "../../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Destination endpoint host state enumeration type.
    */
    enum class HostState : byte_t
    {
        unknown,  // Host down or unresponsive
        closed,   // Host rejected connection
        open      // Host accepted connection
    };
}

#endif // !HOST_STATE_H
