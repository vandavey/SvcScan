/*
*  nullptrex.cpp
*  -------------
*  Source file for nullptr argument exceptions
*/
#include "includes/except/nullptrex.h"
#include "includes/util.h"

/// ***
/// Initialize the object
/// ***
Scan::NullPtrEx::NullPtrEx(const char *arg_ptr)
    : base({arg_ptr}, init_msg()) {
}

/// ***
/// Initialize the object
/// ***
Scan::NullPtrEx::NullPtrEx(const vector_s &vect)
    : base(vect, init_msg()) {
}

/// ***
/// Print exception information to standard error
/// ***
void Scan::NullPtrEx::show() const
{
    Util::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
const std::string Scan::NullPtrEx::name() const noexcept
{
    return NAME;
}

/// ***
/// Get the name of the exception
/// ***
const std::string Scan::NullPtrEx::init_msg() const noexcept
{
    return "Null pointer exception was thrown";
}
