/*
*  argex.cpp
*  ---------
*  Source file for invalid argument exceptions
*/
#include "includes/container/list.h"
#include "includes/except/argex.h"
#include "includes/util.h"

namespace Scan
{
    using string = std::string;
    using list_s = List<string>;
}

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
    this->arg = list_s(vect).join(", ");
    this->msg = msg;
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

/// ***
/// Retrieve exception information as a string
/// ***
const std::string Scan::ArgEx::str() const
{
    const string header(Util::fmt("----[ % ]----", name()));

    // Information line list
    const list_s lines({
        header,
        Util::fmt(" Arg(s) : %", arg.get()),
        Util::fmt(" About  : %", msg.get()),
        string(static_cast<int>(header.size()), '-')
    });
    return static_cast<string>(lines.join());
}

/// ***
/// Retrieve exception information as a string
/// ***
std::string Scan::ArgEx::str()
{
    const string header(Util::fmt("----[ % ]----", name()));

    // Information line list
    const list_s lines({
        header,
        Util::fmt(" Arg(s) : %", arg.get()),
        Util::fmt(" About  : %", msg.get()),
        string(static_cast<int>(header.size()), '-')
    });
    return static_cast<string>(lines.join());
}
