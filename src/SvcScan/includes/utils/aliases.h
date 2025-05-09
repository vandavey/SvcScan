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
#include <cstdint>
#include <initializer_list>
#include <ios>
#include <istream>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <ranges>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/asio/io_context.hpp>

namespace scan
{
    namespace asio   = boost::asio;
    namespace chrono = std::chrono;
    namespace ranges = std::ranges;
    namespace views  = ranges::views;

    using uint_t  = unsigned int;
    using ulong_t = unsigned long;

    using hours        = chrono::hours;
    using microseconds = chrono::microseconds;
    using milliseconds = chrono::milliseconds;
    using minutes      = chrono::minutes;
    using nanoseconds  = chrono::nanoseconds;
    using seconds      = chrono::seconds;

    using steady_clock        = chrono::steady_clock;
    using steady_time_point_t = steady_clock::time_point;
    using system_clock        = chrono::system_clock;
    using system_time_point_t = system_clock::time_point;

    using c_string_t      = const char[];
    using errc            = std::errc;
    using error_code      = std::error_code;
    using io_context_t    = asio::io_context;
    using istream         = std::istream;
    using ostream         = std::ostream;
    using sstream         = std::stringstream;
    using streamsize      = std::streamsize;
    using string          = std::string;
    using strong_ordering = std::strong_ordering;

    template<class T>
    using allocator = std::allocator<T>;

    template<class AllocT>
    using allocator_traits = std::allocator_traits<AllocT>;

    template<class T, size_t N>
    using array = std::array<T, N>;

    template<size_t N>
    using byte_array_t = array<uint8_t, N>;

    template<class T>
    using decay_t = std::decay_t<T>;

    template<class T>
    using hh_mm_ss = chrono::hh_mm_ss<T>;

    template<class T>
    using initializer_list = std::initializer_list<T>;

    template<class T>
    using invoke_result_t = std::invoke_result_t<T>;

    template<class I>
    using iter_reference_t = std::iter_reference_t<I>;

    template<class I>
    using iter_value_t = std::iter_value_t<I>;

    template<class R>
    using iterator_t = ranges::iterator_t<R>;

    template<class KeyT, class ValueT>
    using map = std::map<KeyT, ValueT>;

    template<class T, class T2>
    using pair = std::pair<T, T2>;

    template<class R>
    using range_reference_t = ranges::range_reference_t<R>;

    template<class R>
    using range_size_t = ranges::range_size_t<R>;

    template<class R>
    using range_value_t = ranges::range_value_t<R>;

    template<class T>
    using remove_cvref_t = std::remove_cvref_t<T>;

    template<class T>
    using remove_reference_t = std::remove_reference_t<T>;

    template<class T>
    using shared_ptr = std::shared_ptr<T>;

    template<size_t N>
    using string_array_t = std::array<string, N>;

    template<class T>
    using unique_ptr = std::unique_ptr<T>;

    template<class T, class AllocT = allocator<T>>
    using vector = std::vector<T, AllocT>;
}

#endif // !SCAN_ALIASES_H
