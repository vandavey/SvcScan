/*
* @file
*     error_aliases.h
* @brief
*     Header file for error and exception type aliases.
*/
#pragma once

#ifndef SCAN_ERROR_ALIASES_H
#define SCAN_ERROR_ALIASES_H

#include <stdexcept>

namespace scan
{
    using invalid_argument = std::invalid_argument;
    using logic_error      = std::logic_error;
    using out_of_range     = std::out_of_range;
    using runtime_error    = std::runtime_error;
}

#endif // !SCAN_ERROR_ALIASES_H
