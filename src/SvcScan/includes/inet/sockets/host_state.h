/*
* @file
*     host_state.h
* @brief
*     Header file for a network host state enumeration.
*/
#pragma once

#ifndef SCAN_HOST_STATE_H
#define SCAN_HOST_STATE_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     Network host state enumeration.
    */
    enum class HostState : uint8_t
    {
        unknown,  // Host down or unresponsive
        closed,   // Host rejected connection
        open      // Host accepted connection
    };
}

#endif // !SCAN_HOST_STATE_H
