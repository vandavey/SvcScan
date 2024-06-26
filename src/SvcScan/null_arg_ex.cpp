/*
* @file
*     null_arg_ex.cpp
* @brief
*     Source file for a null argument exception.
*/
#include <string>
#include "includes/errors/null_arg_ex.h"
#include "includes/utils/util.h"

/**
* @brief
*     Initialize the object.
*/
scan::NullArgEx::NullArgEx(const NullArgEx &t_ex) noexcept : base_t(t_ex)
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullArgEx::NullArgEx(const char *t_argp) : base_t(t_argp, init_msg())
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullArgEx::NullArgEx(const string_vector &t_vect) : base_t(t_vect, init_msg())
{
}

/**
* @brief
*     Initialize the object.
*/
scan::NullArgEx::NullArgEx(const string_vector &t_vect, const string &t_msg)
    : base_t(t_vect, t_msg)
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

/**
* @brief
*     Get the underlying exception name.
*/
std::string scan::NullArgEx::name() const noexcept
{
    return NAME;
}

/**
* @brief
*     Get a description of the exception to pass to the base class.
*/
std::string scan::NullArgEx::init_msg() const noexcept
{
    return "Null argument exception was thrown";
}
