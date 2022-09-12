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
#include <boost/json.hpp>
#include "../containers/svc_table.h"
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
        using json_array  = json::array;
        using json_kind   = json::kind;
        using json_object = json::object;
        using json_value  = json::value;
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
        static string prettify(const json_value &t_value,
                               const string &t_indent = { });

        static string prettify(const json_object &t_object,
                               const string &t_indent = { });

        static string prettify(const json_array &t_array,
                               const string &t_indent = { });

        static string serialize(const json_value &t_value);

        static json_value scan_report(const SvcTable &t_table,
                                      const Timer &t_timer,
                                      const string &t_out_path = { });
    };
}

#endif // !JSON_UTIL_H
