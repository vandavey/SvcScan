/*
*  nullargex.cpp
*  -------------
*  Source file for null argument exceptions
*/
#include "includes/except/nullargex.h"
#include "includes/util.h"

namespace Scan
{
    using std::string;
}

/// ***
/// Initialize the object
/// ***
Scan::NullArgEx::NullArgEx(const vector_s &vect)
    : base(vect, init_msg(vect, false)) {
}

/// ***
/// Initialize the object
/// ***
Scan::NullArgEx::NullArgEx(const vector_s &vect, const bool &is_ptr)
    : base(vect, init_msg(vect, is_ptr)) {
}

/// ***
/// Assignment operator overload
/// ***
Scan::NullArgEx &Scan::NullArgEx::operator=(const NullArgEx &ex) noexcept
{
    arg = ex.arg;
    msg = ex.msg;
    return *this;
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
const std::string Scan::NullArgEx::init_msg(const vector_s &vect,
                                            const bool &is_ptr) const {
    if (is_ptr)
    {
        return "Null pointer exception was thrown";
    }
    return "Null argument exception was thrown";
}
