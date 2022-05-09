/*
*  logic_ex.cpp
*  ------------
*  Source file for a basic logical exception
*/
#include "includes/containers/generic/list.h"
#include "includes/except/logic_ex.h"

/// ***
/// Initialize the object
/// ***
scan::LogicEx::LogicEx(const string &t_caller, const string &t_msg) : base(t_msg)
{
    caller = t_caller;
    msg = t_msg;
}

/// ***
/// Cast operator overload
/// ***
scan::LogicEx::operator string() const
{
    const string header{ "----[ UNHANDLED EXCEPTION ]----" };

    const List<string> error_lines(
    {
        header,
        Util::fstr(" Exception   : %", name()),
        Util::fstr(" Location    : %", caller),
        Util::fstr(" Information : %", msg),
        string(static_cast<int>(header.size()), '-')
    });
    return error_lines.join_lines();
}

/// ***
/// Print exception information to standard error
/// ***
void scan::LogicEx::show() const
{
    StdUtil::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
std::string scan::LogicEx::name() const noexcept
{
    return NAME;
}
