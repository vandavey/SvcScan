/*
*  argex.cpp
*  ---------
*  Source file for invalid argument exceptions
*/
#include "includes/container/list.h"
#include "includes/except/argex.h"
#include "includes/utils/util.h"

/// ***
/// Initialize the object
/// ***
Scan::ArgEx::ArgEx(const char *argp, const string &msg) : base(msg)
{
    if (argp == nullptr)
    {
        throw NullArgEx("argp");
    }
    this->arg = argp;
    this->msg = msg;
}

/// ***
/// Initialize the object
/// ***
Scan::ArgEx::ArgEx(const vector_s &vect, const string &msg) : base(msg)
{
    this->arg = List<string>::join(vect, ", ");
    this->msg = msg;
}

/// ***
/// Cast operator overload
/// ***
Scan::ArgEx::operator const std::string() const
{
    const string header(Util::fmt("----[ % ]----", name()));

    // Return exception string
    return List<string>::join({
        header,
        Util::fmt(" Arg(s) : %", arg.get()),
        Util::fmt(" About  : %", msg.get()),
        string(static_cast<int>(header.size()), '-')
    });
}

/// ***
/// Cast operator overload
/// ***
Scan::ArgEx::operator std::string()
{
    const string header(Util::fmt("----[ % ]----", name()));

    // Return exception string
    return List<string>::join({
        header,
        Util::fmt(" Arg(s) : %", arg.get()),
        Util::fmt(" About  : %", msg.get()),
        string(static_cast<int>(header.size()), '-')
    });
}

/// ***
/// Print exception information to standard error
/// ***
void Scan::ArgEx::show() const
{
    Util::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
const std::string Scan::ArgEx::name() const noexcept
{
    return static_cast<string>(NAME);
}
