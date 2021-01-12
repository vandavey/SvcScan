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
Scan::ArgEx::ArgEx(const ArgEx &ex) : base(NAME)
{
    this->arg = ex.arg.get();
    this->msg = ex.msg.get();
}

/// ***
/// Initialize the object
/// ***
Scan::ArgEx::ArgEx(const string &arg, const string &msg) : base(msg)
{
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
/// Retrieve exception information as a string
/// ***
const std::string Scan::ArgEx::str() const
{
    const string header(Util::fmt("----[ % ]----", NAME));

    // Join info as single string
    string data(Util::join(Util::ctos(Util::LF),{
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
std::string Scan::ArgEx::str()
{
    const string header(Util::fmt("----[ % ]----", NAME));

    // Join as a single string
    string data(Util::join(Util::ctos(Util::LF), {
        header,
        Util::fmt(" About  : %", msg.get()),
        Util::fmt(" Arg(s) : %", arg.get()),
        string(static_cast<int>(header.size()), '-')
    }));
    return data;
}
