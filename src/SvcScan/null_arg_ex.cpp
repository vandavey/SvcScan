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
*     Initialize the object.
*/
scan::NullArgEx::NullArgEx(const char* t_argp) : base_t{t_argp, init_msg()}
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullArgEx::NullArgEx(const vector<string>& t_vect) : base_t{t_vect, init_msg()}
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullArgEx::NullArgEx(const vector<string>& t_vect, const string& t_msg)
    : base_t{t_vect, t_msg}
{
}

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::NullArgEx::show() const
{
    util::except(*this);
}
