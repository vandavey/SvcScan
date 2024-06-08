/*
* @file
*     type_defs.h
* @brief
*     Header file for common type aliases.
*/
#pragma once

#ifndef SCAN_TYPE_DEFS_H
#define SCAN_TYPE_DEFS_H

#include <array>
#include <chrono>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <sdkddkver.h>
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include "../containers/generic/index_pair.h"

namespace scan
{
    namespace asio   = boost::asio;
    namespace chrono = std::chrono;
    namespace error  = asio::error;
    namespace ranges = std::ranges;

    using uchar_t = unsigned char;
    using uint_t  = unsigned int;
    using ulong_t = unsigned long;

    using hours        = chrono::hours;
    using microseconds = chrono::microseconds;
    using milliseconds = chrono::milliseconds;
    using minutes      = chrono::minutes;
    using seconds      = chrono::seconds;

    using cstr_t        = char[];
    using error_code    = boost::system::error_code;
    using io_context    = asio::io_context;
    using istream       = std::istream;
    using ostream       = std::ostream;
    using sstream       = std::stringstream;
    using streamsize    = std::streamsize;
    using string        = std::string;
    using string_vector = std::vector<string>;

    template<class T, size_t N>
    using array = std::array<T, N>;

    template<class T>
    using idx_pairs_t = std::vector<IndexPair<T>>;

    template<class T>
    using invoke_result_t = std::invoke_result_t<T>;

    template<class K, class V>
    using map = std::map<K, V>;

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

    template<class T>
    using vector = std::vector<T>;
}

#endif // !SCAN_TYPE_DEFS_H
