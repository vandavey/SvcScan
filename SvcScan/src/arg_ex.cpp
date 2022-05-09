/*
*  arg_ex.cpp
*  ----------
*  Source file for an invalid argument exception
*/
#include "includes/containers/generic/list.h"
#include "includes/except/arg_ex.h"
#include "includes/except/null_ptr_ex.h"

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
scan::ArgEx::ArgEx(const vector<string> &t_vect, const string &t_msg) : base(t_msg)
{
    arg = List<string>(t_vect).join(", ");
    msg = t_msg;
}

/// ***
/// Cast operator overload
/// ***
scan::ArgEx::operator string() const
{
    const string header{ "----[ UNHANDLED EXCEPTION ]----" };

    const List<string> error_lines(
    {
        header,
        Util::fstr(" Exception   : %", name()),
        Util::fstr(" Argument(s) : %", arg),
        Util::fstr(" Information : %", msg),
        string(static_cast<int>(header.size()), '-')
    });
    return error_lines.join_lines();
}

/// ***
/// Print exception information to standard error
/// ***
void scan::ArgEx::show() const
{
    StdUtil::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
std::string scan::ArgEx::name() const noexcept
{
    return NAME;
}
