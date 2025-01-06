/*
* @file
*     json.cpp
* @brief
*     Source file for JSON formatting and manipulation utilities.
*/
#include <ios>
#include <sstream>
#include <string>
#include <boost/json/serialize.hpp>
#include "includes/console/args.h"
#include "includes/errors/arg_ex.h"
#include "includes/inet/http/http_version.h"
#include "includes/inet/http/message.h"
#include "includes/inet/http/request.h"
#include "includes/inet/http/response.h"
#include "includes/ranges/algo.h"
#include "includes/utils/const_defs.h"
#include "includes/utils/json.h"

/**
* @brief
*     Add HTTP request message details from the given service
*     information to the specified HTTP information JSON object.
*/
void scan::json::add_request(object_t& t_http_obj, const SvcInfo& t_info)
{
    t_http_obj[REQUEST_KEY] = object_t
    {
        {VERSION_KEY, t_info.request.httpv.num_str()},
        {METHOD_KEY,  t_info.request.method_str()},
        {URI_KEY,     t_info.request.uri()},
        {HEADERS_KEY, std::move(make_object(t_info.request.msg_headers()))}
    };
}

/**
* @brief
*     Add HTTP response message details from the given service
*     information to the specified HTTP information JSON object.
*/
void scan::json::add_response(object_t& t_http_obj, const SvcInfo& t_info)
{
    t_http_obj[RESPONSE_KEY] = object_t
    {
        {VERSION_KEY, t_info.response.httpv.num_str()},
        {STATUS_KEY,  t_info.response.status_code()},
        {REASON_KEY,  t_info.response.reason()},
        {HEADERS_KEY, std::move(make_object(t_info.response.msg_headers()))},
        {BODY_KEY,    t_info.response.body()}
    };
}

/**
* @brief
*     Create a new JSON object from the given service information
*     and add it to the specified JSON service information array.
*/
void scan::json::add_service(array_t& t_svc_array, const SvcInfo& t_info)
{
    object_t svc_obj
    {
        {PORT_KEY,     t_info.port()},
        {PROTOCOL_KEY, t_info.proto},
        {STATE_KEY,    t_info.state_str()},
        {SERVICE_KEY,  t_info.service},
        {SUMMARY_KEY,  t_info.summary},
        {BANNER_KEY,   t_info.banner}
    };

    // Add SSL/TLS information
    if (!t_info.cipher.empty())
    {
        svc_obj[CIPHER_SUITE_KEY] = t_info.cipher;
        svc_obj[X509_ISSUER_KEY] = t_info.issuer;
        svc_obj[X509_SUBJECT_KEY] = t_info.subject;
    }

    // Add HTTP request and response information
    if (!t_info.response.msg_headers().empty())
    {
        svc_obj[HTTP_INFO_KEY] = object_t{};

        add_request(svc_obj[HTTP_INFO_KEY].get_object(), t_info);
        add_response(svc_obj[HTTP_INFO_KEY].get_object(), t_info);
    }

    t_svc_array.push_back(svc_obj);
}

/**
* @brief
*     Add the services from the given table to the specified scan report JSON object.
*/
void scan::json::add_services(object_t& t_report_obj, const SvcTable& t_table)
{
    if (!valid_schema(t_report_obj))
    {
        throw ArgEx{"t_report_obj", "Invalid scan report JSON received"};
    }

    object_t& results_obj{t_report_obj[SCAN_RESULTS_KEY].get_object()};
    array_t& svc_array{results_obj[SERVICES_KEY].get_array()};

    // Add service information JSON objects
    for (const SvcInfo& info : t_table.values())
    {
        add_service(svc_array, info);
    }
}

/**
* @brief
*     Determine whether the report schema of the given JSON value is valid.
*/
bool scan::json::valid_schema(const object_t& t_report_obj) noexcept
{
    bool valid{false};
    const value_t* resultsp{t_report_obj.if_contains(SCAN_RESULTS_KEY)};

    if (valid = resultsp != nullptr && resultsp->is_object())
    {
        const value_t* servicesp{resultsp->get_object().if_contains(SERVICES_KEY)};
        valid = servicesp != nullptr && servicesp->is_array();
    }
    return valid;
}

/**
* @brief
*     Serialize the given JSON array to a string and prettify the output data.
*/
std::string scan::json::prettify(const array_t& t_array, const string& t_indent)
{
    sstream stream;
    stream << '[';

    if (!t_array.empty())
    {
        stream << LF;
        string indent{t_indent + algo::pad(INDENT_SIZE)};

        for (array_t::const_iterator it{t_array.begin()}; it != t_array.end(); ++it)
        {
            stream << indent << prettify(*it, indent);

            if (it + 1 != t_array.end())
            {
                stream << ',' << LF;
            }
        }

        indent.resize(indent.size() - INDENT_SIZE);
        stream << LF << indent;
    }
    stream << ']';

    return stream.str();
}

/**
* @brief
*     Serialize the given JSON object to a string and prettify the output data.
*/
std::string scan::json::prettify(const object_t& t_obj, const string& t_indent)
{
    sstream stream;
    stream << '{';

    if (!t_obj.empty())
    {
        stream << LF;
        string indent{t_indent + algo::pad(INDENT_SIZE)};

        for (object_t::const_iterator it{t_obj.begin()}; it != t_obj.end(); ++it)
        {
            stream << algo::fstr("%%: ", indent, serialize(it->key()))
                   << prettify(it->value(), indent);

            if (it + 1 != t_obj.end())
            {
                stream << ',' << LF;
            }
        }

        indent.resize(indent.size() - INDENT_SIZE);
        stream << LF << indent;
    }
    stream << '}';

    return stream.str();
}

/**
* @brief
*     Serialize the given JSON value to a string and prettify the output data.
*/
std::string scan::json::prettify(const value_t& t_value, const string& t_indent)
{
    sstream stream;

    switch (t_value.kind())
    {
        case kind_t::null:
            stream << "null";
            break;
        case kind_t::bool_:
            stream << std::boolalpha << t_value.get_bool() << std::boolalpha;
            break;
        case kind_t::int64:
            stream << t_value.get_int64();
            break;
        case kind_t::uint64:
            stream << t_value.get_uint64();
            break;
        case kind_t::double_:
            stream << t_value.get_double();
            break;
        case kind_t::string:
            stream << serialize(t_value.get_string());
            break;
        case kind_t::array:
            stream << prettify(t_value.get_array(), t_indent);
            break;
        case kind_t::object:
            stream << prettify(t_value.get_object(), t_indent);
            break;
        default:
            break;
    }
    return stream.str();
}

/**
* @brief
*     Serialize the given JSON value to a string.
*/
std::string scan::json::serialize(const value_t& t_value)
{
    return boost::json::serialize(t_value);
}

/**
* @brief
*     Create a new scan report JSON object.
*/
boost::json::object scan::json::scan_report(const SvcTable& t_table,
                                            const Timer& t_timer,
                                            const string& t_out_path)
{
    object_t report_obj
    {
        {APP_INFO_KEY,
            {
                {APP_NAME_KEY, APP},
                {APP_REPO_KEY, REPO}
            }
        },
        {SCAN_SUMMARY_KEY,
            {
                {DURATION_KEY,    t_timer.elapsed()},
                {START_TIME_KEY,  t_timer.start_time()},
                {END_TIME_KEY,    t_timer.end_time()},
                {REPORT_PATH_KEY, t_out_path},
                {EXECUTABLE_KEY,  t_table.args().exe_path},
                {ARGUMENTS_KEY,   std::move(make_array(t_table.args().argv))}
            }
        },
        {SCAN_RESULTS_KEY,
            {
                {TARGET_KEY,   t_table.addr()},
                {SERVICES_KEY, array_t{}}
            }
        }
    };
    add_services(report_obj, t_table);

    return report_obj;
}
