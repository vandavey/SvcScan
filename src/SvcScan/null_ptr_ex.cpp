/*
* @file
*     null_ptr_ex.cpp
* @brief
*     Source file for a null pointer argument exception.
*/
#include "includes/errors/null_ptr_ex.h"
#include "includes/io/std_util.h"

/**
* @brief
*     Initialize the object.
*/
scan::NullPtrEx::NullPtrEx(const NullPtrEx &t_ex) : base_t(t_ex)
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullPtrEx::NullPtrEx(const char *t_argp) : base_t({ t_argp }, init_msg())
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullPtrEx::NullPtrEx(const string_vector &t_vect) : base_t(t_vect, init_msg())
{
}

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::NullPtrEx::show() const
{
    StdUtil::except(*this);
}

/**
* @brief
*     Get the underlying exception name.
*/
std::string scan::NullPtrEx::name() const noexcept
{
    return NAME;
}

/**
* @brief
*     Get a description of the exception to pass to the base class.
*/
std::string scan::NullPtrEx::init_msg() const noexcept
{
    return "Null pointer exception was thrown";
}
