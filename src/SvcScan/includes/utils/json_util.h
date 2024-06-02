/*
* @file
*     json_util.h
* @brief
*     Header file for JSON formatting and manipulation utilities.
*/
#pragma once

#ifndef SCAN_JSON_UTIL_H
#define SCAN_JSON_UTIL_H

#include <sdkddkver.h>
#include <boost/json/array.hpp>
#include <boost/json/kind.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value.hpp>
#include <boost/json/value_ref.hpp>
#include "../containers/svc_table.h"
#include "../io/std_util.h"
#include "../utils/algorithm.h"
#include "../utils/timer.h"
#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief
    *     JSON formatting and manipulation utilities.
    */
    class JsonUtil final
    {
    private:  /* Type Aliases */
        using algo        = Algorithm;
        using array_t     = boost::json::array;
        using kind_t      = boost::json::kind;
        using object_t    = boost::json::object;
        using stdu        = StdUtil;
        using value_ref_t = boost::json::value_ref;
        using value_t     = boost::json::value;

    public:  /* Constructors & Destructor */
        JsonUtil() = delete;
        JsonUtil(const JsonUtil &) = delete;
        JsonUtil(JsonUtil &&) = delete;

        virtual ~JsonUtil() = default;

    public:  /* Operators */
        JsonUtil &operator=(const JsonUtil &) = default;
        JsonUtil &operator=(JsonUtil &&) = default;

    public:  /* Methods */
        static string prettify(const value_t &t_value, const string &t_indent = { });
        static string prettify(const object_t &t_obj, const string &t_indent = { });
        static string prettify(const array_t &t_array, const string &t_indent = { });
        static string serialize(const value_t &t_value);

        static value_t scan_report(const SvcTable &t_table,
                                   const Timer &t_timer,
                                   const string &t_out_path = { });

    private:  /* Methods */
        static void add_request(object_t &t_http_obj, const SvcInfo &t_info);
        static void add_response(object_t &t_http_obj, const SvcInfo &t_info);
        static void add_service(array_t &t_svc_array, const SvcInfo &t_info);
        static void add_services(value_t &t_report_val, const SvcTable &t_table);

        static bool valid_array(const value_t *t_valuep, const bool &t_empty_ok = false);
        static bool valid_object(const value_t *t_valuep, const bool &t_empty_ok = false);
        static bool valid_schema(value_t &t_report_val);

        template<Range R>
        static array_t make_array(const R &t_range);

        static object_t make_object(const header_map &t_headers);
    };
}

/**
* @brief
*     Create a JSON array with the values from the given range.
*/
template<scan::Range R>
inline boost::json::array scan::JsonUtil::make_array(const R &t_range)
{
    array_t json_array;

    for (const range_value_t<R> &value : t_range)
    {
        json_array.emplace_back(value);
    }
    return json_array;
}

#endif // !SCAN_JSON_UTIL_H
