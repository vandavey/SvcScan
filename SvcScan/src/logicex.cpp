/*
*  logicex.cpp
*  -----------
*  Source file for basic logic exception
*/
#include "includes/conio/stdutil.h"
#include "includes/containers/generic/list.h"
#include "includes/except/logicex.h"

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

    // Return exception string
    return List<string>::join_lines(
    {
        header,
        Util::fstr(" Exception   : %", name()),
        Util::fstr(" Location    : %", caller),
        Util::fstr(" Information : %", msg),
        string(static_cast<int>(header.size()), '-')
    });
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
