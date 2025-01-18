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

    /// @brief  TCP client disconnected error message.
    constexpr c_string_t CLIENT_DISCONNECTED_MSG = "TCP client currently disconnected.";

    /// @brief  Windows console API failure error message.
    constexpr c_string_t CONSOLE_API_FAILED_FMT_MSG = "Console API failure occurred: %.";

    /// @brief  Empty HTTP header error message.
    constexpr c_string_t EMPTY_HEADER_FMT_MSG = "Required HTTP header empty: '%'.";

    /// @brief  Empty HTTP header map error message.
    constexpr c_string_t EMPTY_HEADER_MAP_MSG = "HTTP header map cannot be empty.";

    /// @brief  Empty vector error message.
    constexpr c_string_t EMPTY_VECTOR_MSG = "The specified vector cannot be empty.";

    /// @brief  Error message details header.
    constexpr c_string_t ERROR_MSG_HEADER = "UNHANDLED EXCEPTION";

    /// @brief  Exception details key.
    constexpr c_string_t EXCEPTION_KEY = "Exception";

    /// @brief  File stream closed error message.
    constexpr c_string_t FILE_CLOSED_MSG = "File stream is currently closed.";

    /// @brief  File not found error message.
    constexpr c_string_t FILE_NOT_FOUND_MSG = "The specified file path does not exist.";

    /// @brief  Unpermitted file operation error message.
    constexpr c_string_t FILE_OP_UNPERMITTED_MSG = "Unpermitted file operation.";

    /// @brief  File open failure error message.
    constexpr c_string_t FILE_OPEN_FAILED_MSG = "Failed to open file stream.";

    /// @brief  Information details key.
    constexpr c_string_t INFORMATION_KEY = "Information";

    /// @brief  Invalid IPv4 address error message.
    constexpr c_string_t INVALID_ADDR_MSG = "Invalid IPv4 address specified.";

    /// @brief  Invalid argument count error message.
    constexpr c_string_t INVALID_ARG_COUNT_MSG = "Invalid argument count specified.";

    /// @brief  Invalid argument index error message.
    constexpr c_string_t INVALID_ARG_INDEX_MSG = "Invalid argument index specified.";

    /// @brief  Invalid IPv4 endpoint error message.
    constexpr c_string_t INVALID_ENDPOINT_MSG = "Invalid IPv4 endpoint specified.";

    /// @brief  Invalid range iterator combination error message.
    constexpr c_string_t INVALID_ITER_COMBO_MSG = "Invalid range iterator combination.";

    /// @brief  Invalid range iterator error message.
    constexpr c_string_t INVALID_ITER_MSG = "Invalid range iterator specified.";

    /// @brief  Invalid offset position error message.
    constexpr c_string_t INVALID_OFFSET_MSG = "Invalid offset position specified.";

    /// @brief  Invalid file path error message.
    constexpr c_string_t INVALID_PATH_MSG = "Invalid file path specified.";

    /// @brief  Invalid network port number(s) error message.
    constexpr c_string_t INVALID_PORTS_MSG = "Port(s) must be between 0 and 65535.";

    /// @brief  Invalid JSON scan report error message.
    constexpr c_string_t INVALID_JSON_REPORT_MSG = "Invalid JSON scan report specified.";

    /// @brief  Invalid embedded resource symbol error message.
    constexpr c_string_t INVALID_RC_SYMBOL_MSG = "Invalid embedded resource symbol.";

    /// @brief  Invalid HTTP request error message.
    constexpr c_string_t INVALID_REQUEST_MSG = "Invalid HTTP request specified.";

    /// @brief  Invalid HTTP response error message.
    constexpr c_string_t INVALID_RESPONSE_MSG = "Invalid HTTP response specified.";

    /// @brief  Invalid network target error message.
    constexpr c_string_t INVALID_TARGET_MSG = "Invalid network target specified.";

    /// @brief  Invalid vector index error message.
    constexpr c_string_t INVALID_VECTOR_INDEX_MSG = "Invalid vector index specified.";

    /// @brief  Error origin location details key.
    constexpr c_string_t LOCATION_KEY = "Location";

    /// @brief  Logic exception type name.
    constexpr c_string_t LOGIC_EX_NAME = "scan::LogicEx";

    /// @brief  Missing HTTP header error message.
    constexpr c_string_t MISSING_HEADER_FMT_MSG = "Required HTTP header missing: '%'.";

    /// @brief  Matching range value not found error message.
    constexpr c_string_t MATCH_NOT_FOUND_MSG = "No matching range value found.";

    /// @brief  String data not integral error message.
    constexpr c_string_t NOT_INTEGRAL_MSG = "String data must be integral.";

    /// @brief  Null argument exception error message.
    constexpr c_string_t NULL_ARG_EX_MSG = "One or more null arguments.";

    /// @brief  Null argument exception type name.
    constexpr c_string_t NULL_ARG_EX_NAME = "scan::NullArgEx";

    /// @brief  Null pointer dereferenced error message.
    constexpr c_string_t NULL_PTR_DEREF_MSG = "Attempted to dereference null pointer.";

    /// @brief  Null pointer argument exception error message.
    constexpr c_string_t NULL_PTR_EX_MSG = "One or more null pointer arguments.";

    /// @brief  Null pointer argument exception type name.
    constexpr c_string_t NULL_PTR_EX_NAME = "scan::NullPtrEx";

    /// @brief  Failure loading embedded resource error message.
    constexpr c_string_t RC_LOAD_FAILED_MSG = "Failed to load embedded resource.";

    /// @brief  Embedded resource not loaded error message.
    constexpr c_string_t RC_NOT_LOADED_MSG = "Embedded resource not loaded.";

    /// @brief  Runtime exception type name.
    constexpr c_string_t RUNTIME_EX_NAME = "scan::RuntimeEx";

    /// @brief  Virtual terminal processing disabled error message.
    constexpr c_string_t VT_DISABLED_MSG = "Virtual terminal processing disabled.";

    /// @brief  Failure enabling virtual terminal processing error message.
    constexpr c_string_t VT_FAILED_MSG = "Failure enabling virtual terminal processing.";
}

#endif // !SCAN_ERROR_CONST_DEFS_H
