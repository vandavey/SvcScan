/*
*  logicex.cpp
*  -----------
*  Source file for basic logic exception
*/
#include "includes/container/list.h"
#include "includes/except/logicex.h"
#include "includes/utils/util.h"

/// ***
/// Initialize the object
/// ***
scan::LogicEx::LogicEx(const string &t_caller,
                       const string &t_msg) : base(t_msg) {
    caller = t_caller;
    msg = t_msg;
}

/// ***
/// Cast operator overload
/// ***
scan::LogicEx::operator string() const
{
    const string header{ Util::fmt("----[ % ]----", name()) };

    // Return exception string
    return List<string>::join({
        header,
        Util::fmt(" Caller : %", caller.get()),
        Util::fmt(" About  : %", msg.get()),
        string(static_cast<int>(header.size()), '-')
    });
}

/// ***
/// Print exception information to standard error
/// ***
void scan::LogicEx::show() const
{
    Util::except(*this);
}

/// ***
/// Get the name of the exception
/// ***
std::string scan::LogicEx::name() const noexcept
{
    return static_cast<string>(NAME);
}
