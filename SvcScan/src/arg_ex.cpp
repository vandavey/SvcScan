/*
*  arg_ex.cpp
*  ----------
*  Source file for an invalid argument exception
*/
#include "includes/containers/generic/list.h"
#include "includes/errors/arg_ex.h"
#include "includes/errors/null_ptr_ex.h"

/**
* @brief  Initialize the object.
*/
scan::ArgEx::ArgEx(const ArgEx &t_ex) : base_t(t_ex.msg)
{
    arg = t_ex.arg;
    msg = t_ex.msg;
}

/**
* @brief  Initialize the object.
*/
scan::ArgEx::ArgEx(const char *t_argp, const string &t_msg) : base_t(t_msg)
{
    if (t_argp == nullptr)
    {
        throw NullPtrEx{ "t_argp" };
    }
    arg = t_argp;
    msg = t_msg;
}

/**
* @brief  Initialize the object.
*/
scan::ArgEx::ArgEx(const string_vector &t_vect, const string &t_msg) : base_t(t_msg)
{
    arg = algo::join(t_vect, ", ");
    msg = t_msg;
}

/**
* @brief  Cast operator overload.
*/
scan::ArgEx::operator std::string() const
{
    const string header{ "----[ UNHANDLED EXCEPTION ]----" };

    const List<string> error_lines(
    {
        header,
        algo::fstr(" Exception   : %", name()),
        algo::fstr(" Argument(s) : %", arg),
        algo::fstr(" Information : %", msg),
        algo::underline(header.size())
    });
    return error_lines.join_lines();
}

/**
* @brief  Write exception information to the standard error stream.
*/
void scan::ArgEx::show() const
{
    StdUtil::except(*this);
}

/**
* @brief  Get the name of the exception.
*/
std::string scan::ArgEx::name() const noexcept
{
    return &NAME[0];
}
