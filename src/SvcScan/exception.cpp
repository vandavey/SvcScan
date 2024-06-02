/*
* @file
*     exception.cpp
* @brief
*     Source file for an abstract user-defined exception.
*/
#include "includes/errors/exception.h"

/**
* @brief
*     Initialize the object.
*/
scan::Exception::Exception(const string &t_msg) noexcept : base_t()
{
    msg = t_msg;
}

/**
* @brief
*     Get an explanatory string of the exception.
*/
const char *scan::Exception::what() const noexcept
{
    return &msg[0];
}
