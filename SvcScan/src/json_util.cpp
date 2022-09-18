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
    string indent{ t_indent + string(4, ' ') };

    stream << algo::fstr("{%", StdUtil::LF);

    if (!t_obj.empty())
    {
        using object_iterator = object_t::const_iterator;

        // Prettify the key-value pairs
        for (object_iterator it{ t_obj.begin() }; it != t_obj.end(); ++it)
        {
            stream << algo::fstr("%%: ", indent, serialize(it->key()))
                   << prettify(it->value(), indent);

            if (t_obj.end() != it + 1)
            {
                stream << algo::fstr(",%", StdUtil::LF);
            }
        }
    }

    indent.resize(indent.size() - 4);
    stream << algo::fstr("%%}", StdUtil::LF, indent);

    return stream.str();
}

/**
* @brief  Serialize the given JSON array to a string and prettify the output data.
*/
std::string scan::JsonUtil::prettify(const array_t &t_array, const string &t_indent)
{
    sstream stream;
    string indent{ t_indent + string(4, ' ') };

    stream << algo::fstr("[%", StdUtil::LF);

    if (!t_array.empty())
    {
        using array_iterator = array_t::const_iterator;

        // Prettify the array elements
        for (array_iterator it{ t_array.begin() }; it != t_array.end(); ++it)
        {
            stream << algo::fstr("%%", indent, prettify(*it, indent));

            if (t_array.end() != it + 1)
            {
                stream << algo::fstr(",%", StdUtil::LF);
            }
        }
    }

    indent.resize(indent.size() - 4);
    stream << algo::fstr("%%]", StdUtil::LF, indent);

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
    value_t value = {
        {
            "appInfo", object_t
            {
                { "name",       "SvcScan" },
                { "repository", ArgParser::REPO }
            }
        },
        {
            "scanSummary", object_t
            {
                { "duration",   t_timer.elapsed_str() },
                { "startTime",  Timer::timestamp(t_timer.beg_time()) },
                { "endTime",    Timer::timestamp(t_timer.end_time()) },
                { "reportPath", t_out_path }
            }
        },
        {
            "scanResults", array_t
            {
                object_t
                {
                    { "target",   t_table.addr() },
                    { "services", array_t{ } }
                }
            }
        }
    };
    add_services(value.get_object(), t_table);

    return value;
}

/**
* @brief  Add the services in the given service table to the
*         specified scan report JSON object.
*/
void scan::JsonUtil::add_services(object_t &t_report_obj, const SvcTable &t_table)
{
    if (!t_report_obj.empty())
    {
        array_t &result_array{ t_report_obj["scanResults"].get_array() };
        object_t &result_obj{ result_array[0].get_object() };

        for (const SvcInfo &info : t_table)
        {
            if (&info != t_table.begin())
            {
                result_obj["services"].get_array().push_back(
                {
                    { "port",     info.port() },
                    { "protocol", info.proto },
                    { "state",    info.state_str() },
                    { "service",  info.service },
                    { "summary",  info.summary },
                    { "banner",   info.banner }
                });
            }
        }
    }
}
