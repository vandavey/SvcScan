/*
*  null_arg_ex.cpp
*  ---------------
*  Source file for a null argument exception
*/
#include "includes/except/null_arg_ex.h"
#include "includes/io/std_util.h"

/// ***
/// Initialize the object
/// ***
scan::NullArgEx::NullArgEx(const char *t_argp) : base(t_argp, init_msg())
{
}

/// ***
/// Initialize the object
/// ***
scan::NullArgEx::NullArgEx(const vector<string> &t_vect) : base(t_vect, init_msg())
{
}

/// ***
/// Initialize the object
/// ***
scan::NullArgEx::NullArgEx(const vector<string> &t_vect, const string &t_msg)
    : base(t_vect, t_msg) {
}

/// ***
/// Print exception information to standard error
/// ***
void scan::NullArgEx::show() const
{
    StdUtil::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
std::string scan::NullArgEx::name() const noexcept
{
    return NAME;
}

/// ***
/// Get an exception description to pass to the base class
/// ***
std::string scan::NullArgEx::init_msg() const noexcept
{
    return "Null argument exception was thrown";
}
