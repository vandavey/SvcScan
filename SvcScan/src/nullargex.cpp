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
Scan::NullArgEx::NullArgEx(const NullArgEx &ex)
: base(ex.arg.get(), ex.msg.get())
{
    this->m_isptr = false;
    this->arg = ex.arg;
    this->msg = ex.msg;
}

/// ***
/// Initialize the object
/// ***
Scan::NullArgEx::NullArgEx(const vector_s &vect)
: base(vect, get_msg(vect, false))
{
    this->m_isptr = false;
}

Scan::NullArgEx::NullArgEx(const vector_s &vect, const bool &is_ptr)
: base(vect, get_msg(vect, is_ptr))
{
    this->m_isptr = is_ptr;
}

/// ***
/// Print exception information to standard error
/// ***
void Scan::NullArgEx::show() const
{
    Util::except(*this);
}

/// ***
/// Retrieve exception information as a string
/// ***
const std::string Scan::NullArgEx::str() const
{
    const string header(Util::fmt("----[ % ]----", NAME));

    // Join info as single string
    const string data(Util::join(Util::ctos(Util::LF), {
        header,
        Util::fmt(" About  : %", msg.get()),
        Util::fmt(" Arg(s) : %", arg.get()),
        string(static_cast<int>(header.size()), '-')
    }));
    return data;
}

/// ***
/// Retrieve exception information as a string
/// ***
std::string Scan::NullArgEx::str()
{
    const string header(Util::fmt("----[ % ]----", NAME));

    // Join as a single string
    const string data(Util::join(Util::ctos(Util::LF), {
        header,
        Util::fmt(" About  : %", msg.get()),
        Util::fmt(" Arg(s) : %", arg.get()),
        string(static_cast<int>(header.size()), '-')
    }));
    return data;
}

/// ***
/// Get exception information to pass to base class
/// ***
const std::string Scan::NullArgEx::get_msg(const vector_s &vect,
                                           const bool &is_ptr) const {
    if (is_ptr)
    {
        return "Null pointer exception thrown";
    }
    return "Null argument exception thrown";
}
