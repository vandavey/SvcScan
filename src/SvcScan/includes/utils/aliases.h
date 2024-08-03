/*
* @file
*     aliases.h
* @brief
*     Header file for common type aliases.
*/
#pragma once

#ifndef SCAN_ALIASES_H
#define SCAN_ALIASES_H

#include <array>
#include <chrono>
#include <compare>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <sdkddkver.h>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>

namespace scan
{
    namespace asio   = boost::asio;
    namespace chrono = std::chrono;
    namespace ranges = std::ranges;

    using uchar_t = unsigned char;
    using uint_t  = unsigned int;
    using ulong_t = unsigned long;

    using hours        = chrono::hours;
    using microseconds = chrono::microseconds;
    using milliseconds = chrono::milliseconds;
    using minutes      = chrono::minutes;
    using nanoseconds  = chrono::nanoseconds;
    using seconds      = chrono::seconds;

    using cstr_t          = const char[];
    using error_code      = boost::system::error_code;
    using io_context      = asio::io_context;
    using istream         = std::istream;
    using ostream         = std::ostream;
    using sstream         = std::stringstream;
    using streamsize      = std::streamsize;
    using string          = std::string;
    using string_vector   = std::vector<string>;
    using strong_ordering = std::strong_ordering;

    template<class T, size_t N>
    using array = std::array<T, N>;

    template<class T>
    using hh_mm_ss = chrono::hh_mm_ss<T>;

    template<class T>
    using invoke_result_t = std::invoke_result_t<T>;

    template<class K, class V>
    using map = std::map<K, V>;

    template<class T, class T2>
    using pair = std::pair<T, T2>;

    template<class R>
    using range_iterator_t = ranges::iterator_t<R>;

    template<class R>
    using range_value_t = ranges::range_value_t<R>;

    template<class T>
    using shared_ptr = std::shared_ptr<T>;

    template<size_t N>
    using string_array = std::array<string, N>;

    template<class T>
    using unique_ptr = std::unique_ptr<T>;

    template<class T, class A = std::allocator<T>>
    using vector = std::vector<T, A>;
}

#endif // !SCAN_ALIASES_H
