/*
*  task_status.h
*  -------------
*  Header file for task execution status enumeration type
*/
#pragma once

#ifndef TASK_STATUS_H
#define TASK_STATUS_H

#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Task execution status enumeration type.
    */
    enum class TaskStatus : byte_t
    {
        not_started,  // Task has not been executed
        executing,    // Task is currently being executed
        complete      // Task was completely executed
    };
}

#endif // !TASK_STATUS_H
