/*
*  nullargex.cpp
*  -------------
*  Source file for null argument exceptions
*/
#include "includes/except/nullargex.h"
#include "includes/util.h"

namespace Scan
{
    using string = std::string;
}

/// ***
/// Initialize the object
/// ***
Scan::NullArgEx::NullArgEx(const char *arg_ptr)
    : base(arg_ptr, init_msg()) {
}

/// ***
/// Initialize the object
/// ***
Scan::NullArgEx::NullArgEx(const vector_s &vect)
    : base(vect, init_msg()) {
}

/// ***
/// Initialize the object
/// ***
Scan::NullArgEx::NullArgEx(const vector_s &vect, const std::string &msg)
    : base(vect, msg) {
}

/// ***
/// Print exception information to standard error
/// ***
void Scan::NullArgEx::show() const
{
    Util::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
const std::string Scan::NullArgEx::name() const noexcept
{
    return NAME;
}

/// ***
/// Get exception information to pass to base class
/// ***
const std::string Scan::NullArgEx::init_msg() const noexcept
{
    return "Null argument exception was thrown";
}