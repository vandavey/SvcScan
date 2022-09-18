/*
*  json_util.h
*  -----------
*  Header file for JSON formatting and manipulation utilities
*/
#pragma once

#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include <sstream>
#include <string>
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

namespace scan
{
    namespace
    {
        namespace json = boost::json;
    }

    /**
    * @brief  JSON formatting and manipulation utilities.
    */
    class JsonUtil final
    {
    private:  /* Type Aliases */
        using algo        = Algorithm;
        using array_t     = json::array;
        using kind_t      = json::kind;
        using object_t    = json::object;
        using value_t     = json::value;
        using value_ref_t = json::value_ref;
        using stdu        = StdUtil;
        using sstream     = std::stringstream;
        using string      = std::string;

    public:  /* Constructors & Destructor */
        JsonUtil() = delete;
        JsonUtil(const JsonUtil &) = delete;
        JsonUtil(JsonUtil &&) = delete;

        virtual ~JsonUtil() = default;

    public:  /* Operators */
        JsonUtil &operator=(const JsonUtil &) = default;
        JsonUtil &operator=(JsonUtil &&) = default;

    public:  /* Methods */
        static string prettify(const value_t &t_value,
                               const string &t_indent = { });

        static string prettify(const object_t &t_obj, const string &t_indent = { });
        static string prettify(const array_t &t_array, const string &t_indent = { });
        static string serialize(const value_t &t_value);

        static value_t scan_report(const SvcTable &t_table,
                                   const Timer &t_timer,
                                   const string &t_out_path = { });

    private:  /* Methods */
        static void add_services(value_t &t_report_val, const SvcTable &t_table);

        static bool valid_array(const value_t &t_value,
                                const bool &t_allow_empty = false);

        static bool valid_object(const value_t &t_value,
                                 const bool &t_allow_empty = false);

        static bool valid_report(value_t &t_report_val);
    };
}

#endif // !JSON_UTIL_H
