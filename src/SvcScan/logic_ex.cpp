/*
* @file
*     logic_ex.cpp
* @brief
*     Source file for a logic exception.
*/
#include <stdexcept>
#include "includes/console/util.h"
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

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::LogicEx::show() const
{
    util::except(*this);
}
