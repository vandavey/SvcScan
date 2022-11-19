/*
*  type_defs.h
*  -----------
*  Header file for common type aliases and constants
*/
#pragma once

#ifndef TYPE_DEFS_H
#define TYPE_DEFS_H

#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <sdkddkver.h>
#include <boost/asio/io_context.hpp>
#include <boost/system/error_code.hpp>

namespace scan
{
    namespace asio   = boost::asio;
    namespace chrono = std::chrono;
    namespace error  = asio::error;
    namespace ranges = std::ranges;
    namespace views  = ranges::views;

    using byte_t  = uint8_t;
    using uint_t  = unsigned int;
    using ulong_t = unsigned long;

    using hours        = chrono::hours;
    using microseconds = chrono::microseconds;
    using milliseconds = chrono::milliseconds;
    using minutes      = chrono::minutes;
    using seconds      = chrono::seconds;

    using error_code    = boost::system::error_code;
    using io_context    = asio::io_context;
    using istream       = std::istream;
    using ostream       = std::ostream;
    using sstream       = std::stringstream;
    using streamsize    = std::streamsize;
    using string        = std::string;
    using string_vector = std::vector<string>;
    using string_view   = std::string_view;

    template<class T, size_t N>
    using array = std::array<T, N>;

    template<size_t N>
    using cstr_t = array<char, N>;

    template<class T>
    using invoke_result_t = std::invoke_result_t<T>;

    template<class K, class V>
    using map = std::map<K, V>;

    template<class R>
    using range_value_t = ranges::range_value_t<R>;

    template<class T>
    using shared_ptr = std::shared_ptr<T>;

    template<size_t N>
    using string_array = std::array<string, N>;

    template<class T>
    using unique_ptr = std::unique_ptr<T>;

    template<class T>
    using vector = std::vector<T>;

    constexpr byte_t BYTE_LEN  = 8U;           // Byte size (in bits)
    constexpr byte_t BYTE_MAX  = 0xFF;         // Maximum byte value
    constexpr byte_t BYTE_NULL = 0x00;         // Null (minimum) byte value

    constexpr char CHAR_NULL = '\0';           // Null string character

    constexpr cstr_t<2> CR   = { "\r" };       // Carriage return
    constexpr cstr_t<2> LF   = { "\n" };       // Line feed
    constexpr cstr_t<3> CRLF = { "\r\n" };     // Carriage return line feed
    constexpr cstr_t<8> APP  = { "SvcScan" };  // Application name

    // Application repository URL
    constexpr cstr_t<36> REPO = { "https://github.com/vandavey/SvcScan" };
}

#endif // !TYPE_DEFS_H
