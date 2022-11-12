/*
*  json_util.cpp
*  -------------
*  Source file for JSON formatting and manipulation utilities
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

        stream << &LF[0];
        string indent{ t_indent + string(4, ' ') };

        // Prettify the JSON key-value pairs
        for (iterator_t it{ t_obj.begin() }; it != t_obj.end(); ++it)
        {
            stream << algo::fstr("%%: ", indent, serialize(it->key()))
                   << prettify(it->value(), indent);

            if (it + 1 != t_obj.end())
            {
                stream << algo::fstr(",%", &LF[0]);
            }
        }

        indent.resize(indent.size() - 4);
        stream << &LF[0] << indent;
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

        stream << &LF[0];
        string indent{ t_indent + string(4, ' ') };

        // Prettify the JSON array elements
        for (iterator_t it{ t_array.begin() }; it != t_array.end(); ++it)
        {
            stream << indent << prettify(*it, indent);

            if (it + 1 != t_array.end())
            {
                stream << algo::fstr(",%", &LF[0]);
            }
        }

        indent.resize(indent.size() - 4);
        stream << &LF[0] << indent;
    }
    stream << "]";

    return stream.str();
}

/**
* @brief  Serialize the given JSON value to a string.
*/
std::string scan::JsonUtil::serialize(const value_t &t_value)
{
    return json::serialize(t_value);
}

/**
* @brief  Create a new scan report JSON object.
*/
boost::json::value scan::JsonUtil::scan_report(const SvcTable &t_table,
                                               const Timer &t_timer,
                                               const string &t_out_path) {
    value_t report_value
    {
        value_ref_t
        {
            "appInfo", value_t
            {
                value_ref_t{ "name",       &APP[0] },
                value_ref_t{ "repository", &REPO[0] }
            }
        },
        value_ref_t
        {
            "scanSummary", value_t
            {
                value_ref_t{ "duration",   t_timer.elapsed_str() },
                value_ref_t{ "startTime",  Timer::timestamp(t_timer.beg_time()) },
                value_ref_t{ "endTime",    Timer::timestamp(t_timer.end_time()) },
                value_ref_t{ "reportPath", t_out_path }
            }
        },
        value_ref_t
        {
            "scanResults", array_t
            {
                value_t
                {
                    value_ref_t{ "target",   t_table.addr() },
                    value_ref_t{ "services", array_t{ } }
                }
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
        value_ref_t{ "version", t_info.req_httpv.num_str() },
        value_ref_t{ "method",  algo::to_string(t_info.req_method) },
        value_ref_t{ "uri",     t_info.req_uri },
        value_ref_t{ "headers", object_t{ } }
    };

    object_t &req_obj{ t_http_obj["request"].get_object() };

    // Add the HTTP request message headers
    for (const header_t &header : t_info.req_headers)
    {
        req_obj["headers"].get_object()[header.first] = header.second;
    }
}

/**
* @brief  Add HTTP response message information from the given service
*         information to the specified HTTP information JSON object.
*/
void scan::JsonUtil::add_response(object_t &t_http_obj, const SvcInfo &t_info)
{
    t_http_obj["response"] = value_t
    {
        value_ref_t{ "version", t_info.resp_httpv.num_str() },
        value_ref_t{ "status",  static_cast<int>(t_info.resp_status) },
        value_ref_t{ "reason",  algo::to_string(t_info.resp_status) },
        value_ref_t{ "headers", object_t{ } }
    };

    object_t &resp_obj{ t_http_obj["response"].get_object() };

    // Add the HTTP response message headers
    for (const header_t &header : t_info.resp_headers)
    {
        resp_obj["headers"].get_object()[header.first] = header.second;
    }
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

    // Add HTTP request and response information
    if (!t_info.resp_headers.empty())
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
    if (!valid_report(t_report_val))
    {
        throw ArgEx{ "t_report_val", "Invalid scan report JSON received" };
    }

    array_t &result_array{ t_report_val.get_object()["scanResults"].get_array() };
    array_t &svc_array{ result_array[0].get_object()["services"].get_array() };

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
* @brief  Determine whether the given JSON value is a valid scan report object.
*/
bool scan::JsonUtil::valid_report(value_t &t_report_val)
{
    bool valid{ false };

    if (valid_object(&t_report_val))
    {
        object_t &report_obj{ t_report_val.get_object() };
        value_t *resultsp{ report_obj.if_contains("scanResults") };

        if (valid = valid_array(resultsp))
        {
            value_t &result_val{ resultsp->get_array()[0] };

            if (valid = valid_object(&result_val))
            {
                object_t &result_obj{ result_val.get_object() };
                valid = valid_array(result_obj.if_contains("services"), true);
            }
        }
    }
    return valid;
}
