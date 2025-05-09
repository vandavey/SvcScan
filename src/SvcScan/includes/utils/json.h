/*
* @file
*     json.h
* @brief
*     Header file for JSON formatting and manipulation utilities.
*/
#pragma once

#ifndef SCAN_JSON_H
#define SCAN_JSON_H

#include <string>
#include <utility>
#include <boost/json/array.hpp>
#include <boost/json/kind.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value.hpp>
#include "../concepts/concepts.h"
#include "../inet/services/svc_info.h"
#include "../inet/services/svc_table.h"
#include "aliases.h"
#include "literals.h"
#include "timer.h"

/**
* @brief
*     JSON formatting and manipulation utilities.
*/
namespace scan::json
{
    /**
    * @brief
    *     JSON type aliases.
    */
    inline namespace aliases
    {
        using array_t  = boost::json::array;
        using kind_t   = boost::json::kind;
        using object_t = boost::json::object;
        using value_t  = boost::json::value;
    }

    /**
    * @brief
    *     JSON constant fields.
    */
    inline namespace defs
    {
        /// @brief  String indent size.
        constexpr size_t INDENT_SIZE = 4_sz;

        /// @brief  Application information property key.
        constexpr c_string_t APP_INFO_KEY = "appInfo";

        /// @brief  Application name property key.
        constexpr c_string_t APP_NAME_KEY = "name";

        /// @brief  Application repository property key.
        constexpr c_string_t APP_REPO_KEY = "repository";

        /// @brief  Command-line arguments property key.
        constexpr c_string_t ARGUMENTS_KEY = "arguments";

        /// @brief  Socket banner property key.
        constexpr c_string_t BANNER_KEY = "banner";

        /// @brief  HTTP message body property key.
        constexpr c_string_t BODY_KEY = "body";

        /// @brief  SSL/TLS cipher suite property key.
        constexpr c_string_t CIPHER_SUITE_KEY = "cipherSuite";

        /// @brief  Scan duration property key.
        constexpr c_string_t DURATION_KEY = "duration";

        /// @brief  Scan end time property key.
        constexpr c_string_t END_TIME_KEY = "endTime";

        /// @brief  Executable path property key.
        constexpr c_string_t EXECUTABLE_KEY = "executable";

        /// @brief  HTTP message headers property key.
        constexpr c_string_t HEADERS_KEY = "headers";

        /// @brief  HTTP information property key.
        constexpr c_string_t HTTP_INFO_KEY = "httpInfo";

        /// @brief  HTTP request method property key.
        constexpr c_string_t METHOD_KEY = "method";

        /// @brief  Network port number property key.
        constexpr c_string_t PORT_KEY = "port";

        /// @brief  Network protocol property key.
        constexpr c_string_t PROTOCOL_KEY = "protocol";

        /// @brief  HTTP response reason phrase property key.
        constexpr c_string_t REASON_KEY = "reason";

        /// @brief  Scan report path property key.
        constexpr c_string_t REPORT_PATH_KEY = "reportPath";

        /// @brief  HTTP request information property key.
        constexpr c_string_t REQUEST_KEY = "request";

        /// @brief  HTTP response information property key.
        constexpr c_string_t RESPONSE_KEY = "response";

        /// @brief  Scan results property key.
        constexpr c_string_t SCAN_RESULTS_KEY = "scanResults";

        /// @brief  Scan summary property key.
        constexpr c_string_t SCAN_SUMMARY_KEY = "scanSummary";

        /// @brief  Network service name property key.
        constexpr c_string_t SERVICE_KEY = "service";

        /// @brief  Network services name property key.
        constexpr c_string_t SERVICES_KEY = "services";

        /// @brief  Scan start time property key.
        constexpr c_string_t START_TIME_KEY = "startTime";

        /// @brief  Host state property key.
        constexpr c_string_t STATE_KEY = "state";

        /// @brief  HTTP response status property key.
        constexpr c_string_t STATUS_KEY = "status";

        /// @brief  Network service summary property key.
        constexpr c_string_t SUMMARY_KEY = "summary";

        /// @brief  Scan target name property key.
        constexpr c_string_t TARGET_KEY = "target";

        /// @brief  HTTP request URI property key.
        constexpr c_string_t URI_KEY = "uri";

        /// @brief  HTTP protocol version property key.
        constexpr c_string_t VERSION_KEY = "version";

        /// @brief  X.509 certificate issuer property key.
        constexpr c_string_t X509_ISSUER_KEY = "x509Issuer";

        /// @brief  X.509 certificate subject property key.
        constexpr c_string_t X509_SUBJECT_KEY = "x509Subject";
    }

    /**
    * @brief
    *     Outdent the given data by one indentation level.
    */
    constexpr string& outdent(string& t_data)
    {
        t_data.resize(t_data.size() <= INDENT_SIZE ? 0_sz : t_data.size() - INDENT_SIZE);
        return t_data;
    }

    void add_request(object_t& t_http_obj, const SvcInfo& t_info);
    void add_response(object_t& t_http_obj, const SvcInfo& t_info);
    void add_service(array_t& t_svc_array, const SvcInfo& t_info);
    void add_services(object_t& t_report_obj, const SvcTable& t_table);

    bool valid_schema(const object_t& t_report_obj) noexcept;

    string prettify(const array_t& t_array, const string& t_indent = {});
    string prettify(const object_t& t_obj, const string& t_indent = {});
    string prettify(const value_t& t_value, const string& t_indent = {});
    string serialize(const value_t& t_value);

    array_t make_array(const Range auto& t_range);
    object_t make_object(const StringMap auto& t_map);

    object_t scan_report(const SvcTable& t_table,
                         const Timer& t_timer,
                         const string& t_out_path = {});
}

/**
* @brief
*     Create a JSON array with the values from the given range.
*/
inline boost::json::array scan::json::make_array(const Range auto& t_range)
{
    array_t json_array;

    for (const auto& value : t_range)
    {
        json_array.emplace_back(value);
    }
    return std::move(json_array);
}

/**
* @brief
*     Create a JSON object from the key-value pairs in the given map.
*/
inline boost::json::object scan::json::make_object(const StringMap auto& t_map)
{
    object_t json_obj;

    for (const StringPair auto& pair : t_map)
    {
        json_obj[pair.first] = pair.second;
    }
    return std::move(json_obj);
}

#endif // !SCAN_JSON_H
