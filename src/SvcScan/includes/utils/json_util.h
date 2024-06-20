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
#include "../concepts/concepts.h"
#include "../containers/svc_table.h"
#include "../inet/net_alias.h"
#include "../inet/sockets/svc_info.h"
#include "../io/std_util.h"
#include "alias.h"
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
        using array_t     = boost::json::array;
        using kind_t      = boost::json::kind;
        using object_t    = boost::json::object;
        using stdu        = StdUtil;
        using value_ref_t = boost::json::value_ref;
        using value_t     = boost::json::value;
    }

    /**
    * @brief
    *     JSON constant fields.
    */
    inline namespace defs
    {
        /// @brief  Application information property key.
        constexpr cstr_t APP_INFO_KEY = "appInfo";

        /// @brief  Application name property key.
        constexpr cstr_t APP_NAME_KEY = "name";

        /// @brief  Application repository property key.
        constexpr cstr_t APP_REPO_KEY = "repository";

        /// @brief  Command-line arguments property key.
        constexpr cstr_t ARGUMENTS_KEY = "arguments";

        /// @brief  Socket banner property key.
        constexpr cstr_t BANNER_KEY = "banner";

        /// @brief  HTTP message body property key.
        constexpr cstr_t BODY_KEY = "body";

        /// @brief  SSL/TLS cipher suite property key.
        constexpr cstr_t CIPHER_SUITE_KEY = "cipherSuite";

        /// @brief  Scan duration property key.
        constexpr cstr_t DURATION_KEY = "duration";

        /// @brief  Scan end time property key.
        constexpr cstr_t END_TIME_KEY = "endTime";

        /// @brief  Executable path property key.
        constexpr cstr_t EXECUTABLE_KEY = "executable";

        /// @brief  HTTP message headers property key.
        constexpr cstr_t HEADERS_KEY = "headers";

        /// @brief  HTTP information property key.
        constexpr cstr_t HTTP_INFO_KEY = "httpInfo";

        /// @brief  HTTP request method property key.
        constexpr cstr_t METHOD_KEY = "method";

        /// @brief  Network port number property key.
        constexpr cstr_t PORT_KEY = "port";

        /// @brief  Network protocol property key.
        constexpr cstr_t PROTOCOL_KEY = "protocol";

        /// @brief  HTTP response reason phrase property key.
        constexpr cstr_t REASON_KEY = "reason";

        /// @brief  Scan report path property key.
        constexpr cstr_t REPORT_PATH_KEY = "reportPath";

        /// @brief  HTTP request information property key.
        constexpr cstr_t REQUEST_KEY = "request";

        /// @brief  HTTP response information property key.
        constexpr cstr_t RESPONSE_KEY = "response";

        /// @brief  Scan results property key.
        constexpr cstr_t SCAN_RESULTS_KEY = "scanResults";

        /// @brief  Scan summary property key.
        constexpr cstr_t SCAN_SUMMARY_KEY = "scanSummary";

        /// @brief  Network service name property key.
        constexpr cstr_t SERVICE_KEY = "service";

        /// @brief  Network services name property key.
        constexpr cstr_t SERVICES_KEY = "services";

        /// @brief  Scan start time property key.
        constexpr cstr_t START_TIME_KEY = "startTime";

        /// @brief  Host state property key.
        constexpr cstr_t STATE_KEY = "state";

        /// @brief  HTTP response status property key.
        constexpr cstr_t STATUS_KEY = "status";

        /// @brief  Network service summary property key.
        constexpr cstr_t SUMMARY_KEY = "summary";

        /// @brief  Scan target name property key.
        constexpr cstr_t TARGET_KEY = "target";

        /// @brief  HTTP request URI property key.
        constexpr cstr_t URI_KEY = "uri";

        /// @brief  HTTP protocol version property key.
        constexpr cstr_t VERSION_KEY = "version";

        /// @brief  X.509 certificate issuer property key.
        constexpr cstr_t X509_ISSUER_KEY = "x509Issuer";

        /// @brief  X.509 certificate subject property key.
        constexpr cstr_t X509_SUBJECT_KEY = "x509Subject";
    }

    void add_request(object_t &t_http_obj, const SvcInfo &t_info);
    void add_response(object_t &t_http_obj, const SvcInfo &t_info);
    void add_service(array_t &t_svc_array, const SvcInfo &t_info);
    void add_services(value_t &t_report_val, const SvcTable &t_table);

    bool valid_array(const value_t *t_valuep, const bool &t_empty_ok = false) noexcept;
    bool valid_object(const value_t *t_valuep, const bool &t_empty_ok = false) noexcept;
    bool valid_schema(value_t &t_report_val) noexcept;

    string prettify(const array_t &t_array, const string &t_indent = {});
    string prettify(const object_t &t_obj, const string &t_indent = {});
    string prettify(const value_t &t_value, const string &t_indent = {});
    string serialize(const value_t &t_value);

    template<Range R>
    array_t make_array(const R &t_range);

    object_t make_object(const header_map &t_headers);

    value_t scan_report(const SvcTable &t_table,
                        const Timer &t_timer,
                        const string &t_out_path = {});
}

/**
* @brief
*     Create a JSON array with the values from the given range.
*/
template<scan::Range R>
inline boost::json::array scan::json::make_array(const R &t_range)
{
    array_t json_array;

    for (const range_value_t<R> &value : t_range)
    {
        json_array.emplace_back(value);
    }
    return json_array;
}

#endif // !SCAN_JSON_UTIL_H
