/*
* @file
*     exception.cpp
* @brief
*     Source file for a virtual user-defined exception.
*/
#include "includes/errors/exception.h"

/**
* @brief
*     Initialize the object.
*/
scan::Exception::Exception(const string& t_msg) noexcept : base_t{}
{
    msg = t_msg;
}
