/*
*  nullptrex.cpp
*  -------------
*  Source file for nullptr argument exceptions
*/
#include "includes/except/nullptrex.h"
#include "includes/io/stdutil.h"

/// ***
/// Initialize the object
/// ***
scan::NullPtrEx::NullPtrEx(const char *t_argp) : base({ t_argp }, init_msg())
{
}

/// ***
/// Initialize the object
/// ***
scan::NullPtrEx::NullPtrEx(const vector<string> &t_vect) : base(t_vect, init_msg())
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
