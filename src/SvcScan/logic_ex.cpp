/*
* @file
*     logic_ex.cpp
* @brief
*     Source file for a logic exception.
*/
#include <stdexcept>
#include "includes/errors/logic_ex.h"

/**
* @brief
*     Initialize the object.
*/
scan::LogicEx::LogicEx(const string& t_msg, const string& t_caller)
    : Exception{t_msg}, logic_error{t_msg}
{
    m_caller = t_caller;
}
