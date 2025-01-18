/*
* @file
*     arg_ex.cpp
* @brief
*     Source file for an invalid argument exception.
*/
#include "includes/console/util.h"
#include "includes/errors/arg_ex.h"

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::ArgEx::show() const
{
    util::except(*this);
}
