/*
* @file
*     json_util.cpp
* @brief
*     Source file for JSON formatting and manipulation utilities.
*/
#include <boost/json/serialize.hpp>
#include "includes/utils/arg_parser.h"
#include "includes/utils/json_util.h"

/**
* @brief  Serialize the given JSON value to a string and prettify the output data.
*/
std::string scan::JsonUtil::prettify(const value_t &t_value, const string &t_indent)
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
* @brief  Serialize the given JSON object to a string and prettify the output data.
*/
std::string scan::JsonUtil::prettify(const object_t &t_obj, const string &t_indent)
{
    sstream stream;
    stream << "{";

    if (!t_obj.empty())
    {
        using iterator_t = object_t::const_iterator;

        stream << LF;
        string indent{ t_indent + string(4, ' ') };

        // Prettify the JSON key-value pairs
        for (iterator_t it{ t_obj.begin() }; it != t_obj.end(); ++it)
        {
            stream << algo::fstr("%%: ", indent, serialize(it->key()))
                   << prettify(it->value(), indent);

            if (it + 1 != t_obj.end())
            {
                stream << algo::fstr(",%", LF);
            }
        }

        indent.resize(indent.size() - 4);
        stream << LF << indent;
    }
    stream << "}";

    return stream.str();
}

/**
* @brief  Serialize the given JSON array to a string and prettify the output data.
*/
std::string scan::JsonUtil::prettify(const array_t &t_array, const string &t_indent)
{
    sstream stream;
    stream << "[";

    if (!t_array.empty())
    {
        using iterator_t = array_t::const_iterator;

        stream << LF;
        string indent{ t_indent + string(4, ' ') };

        // Prettify the JSON array elements
        for (iterator_t it{ t_array.begin() }; it != t_array.end(); ++it)
        {
            stream << indent << prettify(*it, indent);

            if (it + 1 != t_array.end())
            {
                stream << algo::fstr(",%", LF);
            }
        }

        indent.resize(indent.size() - 4);
        stream << LF << indent;
    }
    stream << "]";

    return stream.str();
}

/**
* @brief  Serialize the given JSON value to a string.
*/
std::string scan::JsonUtil::serialize(const value_t &t_value)
{
    return boost::json::serialize(t_value);
}

/**
* @brief  Create a new scan report JSON object.
*/
boost::json::value scan::JsonUtil::scan_report(const SvcTable &t_table,
                                               const Timer &t_timer,
                                               const string &t_out_path)
{
    value_t report_value
    {
        value_ref_t
        {
            "appInfo", value_t
            {
                value_ref_t{ "name",       APP },
                value_ref_t{ "repository", REPO }
            }
        },
        value_ref_t
        {
            "scanSummary", value_t
            {
                value_ref_t{ "duration",   t_timer.elapsed_str() },
                value_ref_t{ "startTime",  t_timer.beg_timestamp() },
                value_ref_t{ "endTime",    t_timer.end_timestamp() },
                value_ref_t{ "reportPath", t_out_path },
                value_ref_t{ "executable", t_table.args().exe_path },
                value_ref_t{ "arguments",  std::move(make_array(t_table.args().argv)) }
            }
        },
        value_ref_t
        {
            "scanResults", value_t
            {
                value_ref_t{ "target",   t_table.addr() },
                value_ref_t{ "services", array_t{ } }
            }
        }
    };
    add_services(report_value, t_table);

    return report_value;
}

/**
* @brief  Add HTTP request message information from the given service
*         information to the specified HTTP information JSON object.
*/
void scan::JsonUtil::add_request(object_t &t_http_obj, const SvcInfo &t_info)
{
    t_http_obj["request"] = value_t
    {
        value_ref_t{ "version", t_info.request.httpv.num_str() },
        value_ref_t{ "method",  t_info.request.method_str() },
        value_ref_t{ "uri",     t_info.request.uri() },
        value_ref_t{ "headers", std::move(make_object(t_info.request.msg_headers())) }
    };
}

/**
* @brief  Add HTTP response message information from the given service
*         information to the specified HTTP information JSON object.
*/
void scan::JsonUtil::add_response(object_t &t_http_obj, const SvcInfo &t_info)
{
    t_http_obj["response"] = value_t
    {
        value_ref_t{ "version", t_info.response.httpv.num_str() },
        value_ref_t{ "status",  t_info.response.status_code() },
        value_ref_t{ "reason",  t_info.response.reason() },
        value_ref_t{ "headers", std::move(make_object(t_info.response.msg_headers())) },
        value_ref_t{ "body",    t_info.response.body() }
    };
}

/**
* @brief  Create a new JSON object from the given service information
*         and add it to the specified JSON service information array.
*/
void scan::JsonUtil::add_service(array_t &t_svc_array, const SvcInfo &t_info)
{
    value_t svc_value
    {
        value_ref_t{ "port",     t_info.port() },
        value_ref_t{ "protocol", t_info.proto },
        value_ref_t{ "state",    t_info.state_str() },
        value_ref_t{ "service",  t_info.service },
        value_ref_t{ "summary",  t_info.summary },
        value_ref_t{ "banner",   t_info.banner }
    };

    // Add SSL/TLS information
    if (!t_info.cipher.empty())
    {
        svc_value.get_object()["cipherSuite"] = t_info.cipher;
        svc_value.get_object()["x509Issuer"] = t_info.issuer;
        svc_value.get_object()["x509Subject"] = t_info.subject;
    }

    // Add HTTP request and response information
    if (!t_info.response.msg_headers().empty())
    {
        svc_value.get_object()["httpInfo"] = object_t{ };
        object_t &svc_obj{ svc_value.get_object() };

        add_request(svc_obj["httpInfo"].get_object(), t_info);
        add_response(svc_obj["httpInfo"].get_object(), t_info);
    }

    t_svc_array.push_back(svc_value);
}

/**
* @brief  Add the services in the given service table to the
*         specified scan report JSON object.
*/
void scan::JsonUtil::add_services(value_t &t_report_val, const SvcTable &t_table)
{
    if (!valid_schema(t_report_val))
    {
        throw ArgEx{ "t_report_val", "Invalid scan report JSON received" };
    }

    object_t &results_obj{ t_report_val.get_object()["scanResults"].get_object() };
    array_t &svc_array{ results_obj["services"].get_array() };

    // Add service information JSON objects
    for (const SvcInfo &info : t_table)
    {
        if (&info != t_table.begin())
        {
            add_service(svc_array, info);
        }
    }
}

/**
* @brief  Determine whether the given JSON value is a valid array.
*/
bool scan::JsonUtil::valid_array(const value_t *t_valuep, const bool &t_empty_ok)
{
    bool valid{ false };

    if (t_valuep != nullptr)
    {
        const bool is_array{ t_valuep->is_array() };
        valid = t_empty_ok ? is_array : is_array && t_valuep->get_array().size() > 0;
    }
    return valid;
}

/**
* @brief  Determine whether the given JSON value is a valid object.
*/
bool scan::JsonUtil::valid_object(const value_t *t_valuep, const bool &t_empty_ok)
{
    bool valid{ false };

    if (t_valuep != nullptr)
    {
        const bool is_obj{ t_valuep->is_object() };
        valid = t_empty_ok ? is_obj : is_obj && !t_valuep->get_object().empty();
    }
    return valid;
}

/**
* @brief  Determine whether the report schema of the given JSON value is valid.
*/
bool scan::JsonUtil::valid_schema(value_t &t_report_val)
{
    bool valid{ false };

    if (valid_object(&t_report_val))
    {
        object_t &report_obj{ t_report_val.get_object() };
        value_t *resultsp{ report_obj.if_contains("scanResults") };

        if (valid = valid_object(resultsp))
        {
            object_t &results_obj{ resultsp->get_object() };
            valid = valid_array(results_obj.if_contains("services"), true);
        }
    }
    return valid;
}

/**
* @brief  Create a JSON object with the HTTP headers from the given header map.
*/
boost::json::object scan::JsonUtil::make_object(const header_map &t_headers)
{
    object_t headers_obj;

    for (const header_t &header : t_headers)
    {
        headers_obj[header.first] = header.second;
    }
    return headers_obj;
}
