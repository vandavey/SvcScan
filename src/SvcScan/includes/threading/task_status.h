/*
* @file
*     task_status.h
* @brief
*     Header file for a task execution status enumeration.
*/
#pragma once

#ifndef SCAN_TASK_STATUS_H
#define SCAN_TASK_STATUS_H

#include <cstdint>

namespace scan
{
    /**
    * @brief
    *     Task execution status enumeration.
    */
    enum class TaskStatus : uint8_t
    {
        not_started,  // Task has not been executed
        executing,    // Task is currently being executed
        complete      // Task was completely executed
    };
}

#endif // !SCAN_TASK_STATUS_H
