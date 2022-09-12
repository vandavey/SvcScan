/*
*  task_status.h
*  -------------
*  Header file for task execution status enumeration type
*/
#pragma once

#ifndef TASK_STATUS_H
#define TASK_STATUS_H

namespace scan
{
    /**
    * @brief  Task execution status enumeration type.
    */
    enum class TaskStatus : unsigned int
    {
        not_started,  // Task has not been executed
        executing,    // Task is currently being executed
        complete      // Task was completely executed
    };
}

#endif // !TASK_STATUS_H
