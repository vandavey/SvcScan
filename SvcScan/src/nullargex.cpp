/*
*  nullargex.cpp
*  -------------
*  Source file for null argument exceptions
*/
#include "includes/except/nullargex.h"
#include "includes/io/stdutil.h"

/// ***
/// Initialize the object
/// ***
scan::NullArgEx::NullArgEx(const char *t_argp) :
    base(t_argp, init_msg()) {
}

/// ***
/// Initialize the object
/// ***
scan::NullArgEx::NullArgEx(const vector_s &t_vect) :
    base(t_vect, init_msg()) {
}

/// ***
/// Initialize the object
/// ***
scan::NullArgEx::NullArgEx(const vector_s &t_vect, const string &t_msg) :
    base(t_vect, t_msg) {
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
