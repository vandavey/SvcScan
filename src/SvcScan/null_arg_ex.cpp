/*
* @file
*     null_arg_ex.cpp
* @brief
*     Source file for a null argument exception.
*/
#include "includes/console/util.h"
#include "includes/errors/null_arg_ex.h"

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::NullArgEx::show() const
{
    util::except(*this);
}
