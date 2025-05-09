/*
* @file
*     svc_table.cpp
* @brief
*     Source file for a network application service table.
*/
#include "includes/console/color.h"
#include "includes/console/util.h"
#include "includes/errors/error_const_defs.h"
#include "includes/errors/runtime_ex.h"
#include "includes/inet/services/svc_table.h"
#include "includes/utils/const_defs.h"

/**
* @brief
*     Get the underlying service information as a string.
*     Optionally includes the underlying HTTP responses.
*/
std::string scan::SvcTable::str(bool t_colorize) const
{
    return algo::concat(table_str(t_colorize), LF, details_str(t_colorize));
}

/**
* @brief
*     Get the underlying service information table as a string.
*/
std::string scan::SvcTable::table_str(bool t_colorize) const
{
    sstream stream;

    // Add scan table title
    if (!m_addr.empty())
    {
        stream << util::fmt_title("Target", m_addr, t_colorize) << LF;
    }
    const field_size_map_t size_map{make_size_map()};

    // Add header table record
    const vector<string> header_fields
    {
        algo::pad("PORT", size_map.at(SvcField::port)),
        algo::pad("SERVICE", size_map.at(SvcField::service)),
        algo::pad("STATE", size_map.at(SvcField::state)),
        "INFO"
    };
    const string delim{"   "};

    const string header{algo::join(header_fields, delim)};
    stream << (t_colorize ? util::colorize(header, Color::green) : header) << LF;

    // Add populated table records
    for (const SvcInfo& svc_info : m_list)
    {
        const vector<string> record_fields
        {
            algo::pad(svc_info.port_str(), size_map.at(SvcField::port)),
            algo::pad(svc_info.service, size_map.at(SvcField::service)),
            algo::pad(svc_info.state_str(), size_map.at(SvcField::state)),
            svc_info.summary
        };
        stream << algo::join(record_fields, delim) << LF;
    }

    return stream.str();
}

/**
* @brief
*     Get a constant reference to the underlying command-line arguments.
*/
const scan::Args& scan::SvcTable::args() const
{
    if (m_argsp == nullptr)
    {
        throw RuntimeEx{NULL_PTR_DEREF_MSG, "SvcTable::args"};
    }
    return *m_argsp;
}

/**
* @brief
*     Create a mapping of the underlying service
*     information fields and their maximum sizes.
*/
scan::SvcTable::field_size_map_t scan::SvcTable::make_size_map() const
{
    return field_size_map_t
    {
        {SvcField::port,    max_field_size(SvcField::port)},
        {SvcField::state,   max_field_size(SvcField::state)},
        {SvcField::service, max_field_size(SvcField::service)},
        {SvcField::summary, max_field_size(SvcField::summary)}
    };
}

/**
* @brief
*     Get the details about the underlying services as a string.
*/
std::string scan::SvcTable::details_str(bool t_colorize) const
{
    sstream stream;

    for (const SvcInfo& info : m_list)
    {
        stream << info.details(t_colorize);

        if (&info != &m_list.last())
        {
            stream << LF;
        }
    }
    return stream.str();
}
