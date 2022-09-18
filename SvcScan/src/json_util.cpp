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

        stream << stdu::LF;
        string indent{ t_indent + string(4, ' ') };

        // Prettify the JSON key-value pairs
        for (iterator_t it{ t_obj.begin() }; it != t_obj.end(); ++it)
        {
            stream << algo::fstr("%%: ", indent, serialize(it->key()))
                   << prettify(it->value(), indent);

            if (t_obj.end() != it + 1)
            {
                stream << algo::fstr(",%", stdu::LF);
            }
        }

        indent.resize(indent.size() - 4);
        stream << stdu::LF << indent;
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

        stream << stdu::LF;
        string indent{ t_indent + string(4, ' ') };

        // Prettify the JSON array elements
        for (iterator_t it{ t_array.begin() }; it != t_array.end(); ++it)
        {
            stream << indent << prettify(*it, indent);

            if (t_array.end() != it + 1)
            {
                stream << algo::fstr(",%", stdu::LF);
            }
        }

        indent.resize(indent.size() - 4);
        stream << stdu::LF << indent;
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
                value_ref_t{ "name", ArgParser::APP },
                value_ref_t{ "repo", ArgParser::REPO }
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
* @brief  Add the services in the given service table to the
*         specified scan report JSON object.
*/
void scan::JsonUtil::add_services(value_t &t_report_val, const SvcTable &t_table)
{
    if (!valid_report(t_report_val))
    {
        throw ArgEx{ "t_report_val", "Invalid report JSON received" };
    }

    object_t &report_obj{ t_report_val.get_object() };
    array_t &result_array{ report_obj["scanResults"].get_array() };

    object_t &result_obj{ result_array[0].get_object() };
    array_t &service_array{ result_obj["services"].get_array() };

    // Add service information JSON objects
    for (const SvcInfo &info : t_table)
    {
        if (&info != t_table.begin())
        {
            service_array.push_back(value_t
            {
                value_ref_t{ "port",     info.port() },
                value_ref_t{ "protocol", info.proto },
                value_ref_t{ "state",    info.state_str() },
                value_ref_t{ "service",  info.service },
                value_ref_t{ "summary",  info.summary },
                value_ref_t{ "banner",   info.banner }
            });
        }
    }
}

/**
* @brief  Determine whether the given JSON value is a valid array.
*/
bool scan::JsonUtil::valid_array(const value_t &t_value, const bool &t_allow_empty)
{
    const bool is_array{ t_value.is_array() };
    return t_allow_empty ? is_array : is_array && t_value.get_array().size() > 0;
}

/**
* @brief  Determine whether the given JSON value is a valid object.
*/
bool scan::JsonUtil::valid_object(const value_t &t_value, const bool &t_allow_empty)
{
    const bool is_object{ t_value.is_object() };
    return t_allow_empty ? is_object : is_object && !t_value.get_object().empty();
}

/**
* @brief  Determine whether the given JSON value is a valid scan report object.
*/
bool scan::JsonUtil::valid_report(value_t &t_report_val)
{
    bool valid{ false };

    if (valid_object(t_report_val))
    {
        object_t &report_obj{ t_report_val.get_object() };
        value_t *resultsp{ report_obj.if_contains("scanResults") };

        if (valid = resultsp != nullptr && valid_array(*resultsp))
        {
            array_t &results{ resultsp->get_array() };
            value_t *servicesp{ results[0].get_object().if_contains("services") };

            valid = servicesp != nullptr && valid_array(*servicesp, true);
        }
    }
    return valid;
}
