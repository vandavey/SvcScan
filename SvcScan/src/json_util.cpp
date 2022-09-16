/*
*  json_util.cpp
*  -------------
*  Source file for JSON formatting and manipulation utilities
*/
#include "includes/utils/arg_parser.h"
#include "includes/utils/json_util.h"

/**
* @brief  Serialize the given JSON value to a string and prettify the string data.
*/
std::string scan::JsonUtil::prettify(const json_value &t_value,
                                     const string &t_indent) {
    sstream stream;

    switch (t_value.kind())
    {
        case json_kind::null:
            stream << "null";
            break;
        case json_kind::bool_:
            stream << std::boolalpha << t_value.get_bool() << std::boolalpha;
            break;
        case json_kind::int64:
            stream << t_value.get_int64();
            break;
        case json_kind::uint64:
            stream << t_value.get_uint64();
            break;
        case json_kind::double_:
            stream << t_value.get_double();
            break;
        case json_kind::string:
            stream << serialize(t_value.get_string());
            break;
        case json_kind::array:
            stream << prettify(t_value.get_array(), t_indent);
            break;
        case json_kind::object:
            stream << prettify(t_value.get_object(), t_indent);
            break;
        default:
            break;
    }
    return stream.str();
}

/**
* @brief  Serialize the given JSON object to a string and prettify the string data.
*/
std::string scan::JsonUtil::prettify(const json_object &t_object,
                                     const string &t_indent) {
    sstream stream;
    string indent{ t_indent + string(4, ' ') };

    stream << algo::fstr("{%", StdUtil::LF);

    if (!t_object.empty())
    {
        using object_iterator = json_object::const_iterator;

        // Prettify the key-value pairs
        for (object_iterator it{ t_object.begin() }; it != t_object.end(); ++it)
        {
            stream << algo::fstr("%%: ", indent, serialize(it->key()))
                   << prettify(it->value(), indent);

            if (t_object.end() != it + 1)
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
* @brief  Serialize the given JSON array to a string and prettify the string data.
*/
std::string scan::JsonUtil::prettify(const json_array &t_array,
                                     const string &t_indent) {
    sstream stream;
    string indent{ t_indent + string(4, ' ') };

    stream << algo::fstr("[%", StdUtil::LF);

    if (!t_array.empty())
    {
        using array_iterator = json_array::const_iterator;

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
std::string scan::JsonUtil::serialize(const json_value &t_value)
{
    return json::serialize(t_value);
}

/**
* @brief  Create a new scan report JSON object.
*/
boost::json::value scan::JsonUtil::scan_report(const SvcTable &t_table,
                                               const Timer &t_timer,
                                               const string &t_out_path) {
    json_value value = {
        {
            "appInfo", {
                { "name",       "SvcScan" },
                { "repository", ArgParser::REPO }
            }
        },
        {
            "scanInfo", {
                { "duration",   t_timer.elapsed_str() },
                { "startTime",  Timer::timestamp(t_timer.beg_time()) },
                { "endTime",    Timer::timestamp(t_timer.end_time()) },
                { "reportPath", t_out_path }
            }
        },
        {
            "scanResults", {
                {
                    { "target",   t_table.addr() },
                    { "services", json_array() }
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
void scan::JsonUtil::add_services(json_object &t_report_object,
                                  const SvcTable &t_table) {
    if (!t_report_object.empty())
    {
        json_array &results_arr{ t_report_object["scanResults"].get_array() };
        json_object &result_obj{ results_arr[0].get_object() };

        for (const Record &record : t_table)
        {
            if (&record != t_table.begin())
            {
                result_obj["services"].get_array().push_back(
                {
                    { "port",     record.port_num() },
                    { "protocol", record.proto },
                    { "state",    record.state },
                    { "service",  record.service },
                    { "info",     record.summary }
                });
            }
        }
    }
}
