/*
* @file
*     null_ptr_ex.cpp
* @brief
*     Source file for a null pointer argument exception.
*/
#include "includes/console/util.h"
#include "includes/errors/null_ptr_ex.h"

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::NullPtrEx::show() const
{
    util::except(*this);
}
