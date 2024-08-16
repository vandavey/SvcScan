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
*     Initialize the object.
*/
scan::NullPtrEx::NullPtrEx(const char* t_argp) : base_t{{t_argp}, init_msg()}
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullPtrEx::NullPtrEx(const string_vector& t_vect) : base_t{t_vect, init_msg()}
{
}

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::NullPtrEx::show() const
{
    util::except(*this);
}
