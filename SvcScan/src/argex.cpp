/*
*  argex.cpp
*  ---------
*  Source file for invalid argument exceptions
*/
#include "includes/except/argex.h"
#include "includes/util.h"

namespace Scan
{
    using std::string;
}

/// ***
/// Initialize the object
/// ***
Scan::ArgEx::ArgEx(const char *arg, const string &msg) : base(msg)
{
    if (arg == nullptr)
    {
        throw NullArgEx("arg", true);
    }
    this->arg = arg;
    this->msg = msg;
}

/// ***
/// Initialize the object
/// ***
Scan::ArgEx::ArgEx(const vector_s &vect, const string &msg) : base(msg)
{
    this->arg = Util::join(", ", vect);
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
    return NAME;
}

/// ***
/// Retrieve exception information as a string
/// ***
const std::string Scan::ArgEx::str() const
{
    const string header(Util::fmt("----[ % ]----", name()));

    // Join info as single string
    string data(Util::join(Util::ctos(Util::LF),{
        header,
        Util::fmt(" Arg(s) : %", arg.get()),
        Util::fmt(" About  : %", msg.get()),
        string(static_cast<int>(header.size()), '-')
    }));
    return data;
}

/// ***
/// Retrieve exception information as a string
/// ***
std::string Scan::ArgEx::str()
{
    const string header(Util::fmt("----[ % ]----", name()));

    // Join as a single string
    string data(Util::join(Util::ctos(Util::LF), {
        header,
        Util::fmt(" Arg(s) : %", arg.get()),
        Util::fmt(" About  : %", msg.get()),
        string(static_cast<int>(header.size()), '-')
    }));
    return data;
}
