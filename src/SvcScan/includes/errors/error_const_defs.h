/*
* @file
*     error_const_defs.h
* @brief
*     Header file for error and exception constant expression fields.
*/
#pragma once

#ifndef SCAN_ERROR_CONST_DEFS_H
#define SCAN_ERROR_CONST_DEFS_H

#include "../utils/aliases.h"

namespace scan
{
    /// @brief  Argument exception type name.
    constexpr c_string_t ARG_EX_NAME = "scan::ArgEx";

    /// @brief  Argument(s) details key.
    constexpr c_string_t ARGUMENTS_KEY = "Argument(s)";

    /// @brief  Error message details header.
    constexpr c_string_t ERROR_MSG_HEADER = "UNHANDLED EXCEPTION";

    /// @brief  Exception details key.
    constexpr c_string_t EXCEPTION_KEY = "Exception";

    /// @brief  Information details key.
    constexpr c_string_t INFORMATION_KEY = "Information";

    /// @brief  Error origin location details key.
    constexpr c_string_t LOCATION_KEY = "Location";

    /// @brief  Logic exception type name.
    constexpr c_string_t LOGIC_EX_NAME = "scan::LogicEx";

    /// @brief  Null argument exception error message.
    constexpr c_string_t NULL_ARG_EX_MSG = "One or more null arguments.";

    /// @brief  Null argument exception type name.
    constexpr c_string_t NULL_ARG_EX_NAME = "scan::NullArgEx";

    /// @brief  Null pointer argument exception error message.
    constexpr c_string_t NULL_PTR_EX_MSG = "One or more null pointer arguments.";

    /// @brief  Null pointer argument exception type name.
    constexpr c_string_t NULL_PTR_EX_NAME = "scan::NullPtrEx";

    /// @brief  Runtime exception type name.
    constexpr c_string_t RUNTIME_EX_NAME = "scan::RuntimeEx";
}

#endif // !SCAN_ERROR_CONST_DEFS_H
