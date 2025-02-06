/*
* @file
*     runtime_ex.cpp
* @brief
*     Source file for a runtime exception.
*/
#include <stdexcept>
#include "includes/errors/runtime_ex.h"

/**
* @brief
*     Initialize the object.
*/
scan::RuntimeEx::RuntimeEx(const string& t_msg, const string& t_caller)
    : Exception{t_msg}, runtime_error{t_msg}
{
    m_caller = t_caller;
}
