/*
* @file
*     logic_ex.cpp
* @brief
*     Source file for a logic exception.
*/
#include "includes/errors/logic_ex.h"
#include "includes/utils/util.h"

/**
* @brief
*     Initialize the object.
*/
scan::LogicEx::LogicEx(const string& t_caller, const string& t_msg) : base_t{t_msg}
{
    caller = t_caller;
}

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::LogicEx::show() const
{
    util::except(*this);
}
