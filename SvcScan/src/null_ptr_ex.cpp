/*
*  null_ptr_ex.cpp
*  ---------------
*  Source file for a null pointer argument exception
*/
#include "includes/except/null_ptr_ex.h"
#include "includes/io/std_util.h"

/// ***
/// Initialize the object
/// ***
scan::NullPtrEx::NullPtrEx(const char *t_argp) : base_t({ t_argp }, init_msg())
{
}

/// ***
/// Initialize the object
/// ***
scan::NullPtrEx::NullPtrEx(const vector<string> &t_vect) : base_t(t_vect, init_msg())
{
}

/// ***
/// Print exception information to standard error
/// ***
void scan::NullPtrEx::show() const
{
    StdUtil::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
std::string scan::NullPtrEx::name() const noexcept
{
    return NAME;
}

/// ***
/// Get an exception description to pass to the base class
/// ***
std::string scan::NullPtrEx::init_msg() const noexcept
{
    return "Null pointer exception was thrown";
}
