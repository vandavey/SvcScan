/*
*  argex.cpp
*  ---------
*  Source file for invalid argument exceptions
*/
#include "includes/container/list.h"
#include "includes/except/nullptrex.h"
#include "includes/utils/util.h"

/// ***
/// Initialize the object
/// ***
scan::ArgEx::ArgEx(const char *t_argp, const string &t_msg) : base(t_msg)
{
    if (t_argp == nullptr)
    {
        throw NullPtrEx{ "t_argp" };
    }
    arg = t_argp;
    msg = t_msg;
}

/// ***
/// Initialize the object
/// ***
scan::ArgEx::ArgEx(const vector_s &t_vect, const string &t_msg) : base(t_msg)
{
    arg = List<string>::join(t_vect, ", ");
    msg = t_msg;
}

/// ***
/// Cast operator overload
/// ***
scan::ArgEx::operator string() const
{
    const string header{ Util::fstr("----[ % ]----", name()) };

    // Return exception string
    return List<string>::join({
        header,
        Util::fstr(" Arg(s) : %", arg),
        Util::fstr(" About  : %", msg),
        string(static_cast<int>(header.size()), '-')
    });
}

/// ***
/// Print exception information to standard error
/// ***
void scan::ArgEx::show() const
{
    Util::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
std::string scan::ArgEx::name() const noexcept
{
    return static_cast<string>(NAME);
}
