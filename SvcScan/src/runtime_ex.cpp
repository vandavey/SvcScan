/*
*  runtime_ex.cpp
*  --------------
*  Source file for a runtime exception
*/
#include "includes/containers/generic/list.h"
#include "includes/except/runtime_ex.h"

/**
* @brief  Initialize the object.
*/
scan::RuntimeEx::RuntimeEx(const RuntimeEx &t_ex) : base_t(t_ex.msg)
{
    caller = t_ex.caller;
    msg = t_ex.msg;
}

/**
* @brief  Initialize the object.
*/
scan::RuntimeEx::RuntimeEx(const string &t_caller, const string &t_msg)
    : base_t(t_msg) {

    caller = t_caller;
    msg = t_msg;
}

/**
* @brief  Cast operator overload.
*/
scan::RuntimeEx::operator string() const
{
    const string header{ "----[ UNHANDLED EXCEPTION ]----" };

    const List<string> error_lines(
    {
        header,
        algo::fstr(" Exception   : %", name()),
        algo::fstr(" Location    : %", caller),
        algo::fstr(" Information : %", msg),
        string(static_cast<int>(header.size()), '-')
    });
    return error_lines.join_lines();
}

/**
* @brief  Write exception information to the standard error stream.
*/
void scan::RuntimeEx::show() const
{
    StdUtil::except(*this);
}

/**
* @brief  Get the name of the exception.
*/
std::string scan::RuntimeEx::name() const noexcept
{
    return NAME;
}
