/*
* @file
*     runtime_ex.cpp
* @brief
*     Source file for a runtime exception.
*/
#include "includes/errors/runtime_ex.h"
#include "includes/utils/util.h"

/**
* @brief
*     Initialize the object.
*/
scan::RuntimeEx::RuntimeEx(const string& t_caller, const string& t_msg) : base_t{t_msg}
{
    caller = t_caller;
}

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::RuntimeEx::show() const
{
    util::except(*this);
}
