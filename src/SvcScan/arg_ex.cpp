/*
* @file
*     arg_ex.cpp
* @brief
*     Source file for an invalid argument exception.
*/
#include "includes/console/util.h"
#include "includes/errors/arg_ex.h"
#include "includes/errors/null_ptr_ex.h"
#include "includes/ranges/algo.h"

/**
* @brief
*     Initialize the object.
*/
scan::ArgEx::ArgEx(const char* t_argp, const string& t_msg) : base_t{t_msg}
{
    if (t_argp == nullptr)
    {
        throw NullPtrEx{"t_argp"};
    }
    arg = t_argp;
}

/**
* @brief
*     Initialize the object.
*/
scan::ArgEx::ArgEx(const vector<string>& t_vect, const string& t_msg) : base_t{t_msg}
{
    arg = algo::join(t_vect, ", ");
}

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::ArgEx::show() const
{
    util::except(*this);
}
