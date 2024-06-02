/*
* @file
*     logic_ex.cpp
* @brief
*     Source file for a logic exception.
*/
#include "includes/containers/generic/list.h"
#include "includes/errors/logic_ex.h"

/**
* @brief
*     Initialize the object.
*/
scan::LogicEx::LogicEx(const LogicEx &t_ex) noexcept : base_t(t_ex.msg)
{
    caller = t_ex.caller;
}

/**
* @brief
*     Initialize the object.
*/
scan::LogicEx::LogicEx(const string &t_caller, const string &t_msg) : base_t(t_msg)
{
    caller = t_caller;
}

/**
* @brief
*     Cast operator overload.
*/
scan::LogicEx::operator std::string() const
{
    const string header{ "----[ UNHANDLED EXCEPTION ]----" };

    const List<string> error_lines(
    {
        header,
        algo::fstr(" Exception   : %", name()),
        algo::fstr(" Location    : %", caller),
        algo::fstr(" Information : %", msg),
        algo::underline(header.size())
    });
    return error_lines.join_lines();
}

/**
* @brief
*     Write exception information to the standard error stream.
*/
void scan::LogicEx::show() const
{
    StdUtil::except(*this);
}

/**
* @brief
*     Get the underlying exception name.
*/
std::string scan::LogicEx::name() const noexcept
{
    return NAME;
}
