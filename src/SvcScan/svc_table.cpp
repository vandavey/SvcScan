/*
* @file
*     svc_table.cpp
* @brief
*     Source file for a network application service table.
*/
#include <string>
#include <type_traits>
#include "includes/containers/svc_table.h"
#include "includes/errors/runtime_ex.h"
#include "includes/io/color.h"
#include "includes/utils/algo.h"
#include "includes/utils/const_defs.h"
#include "includes/utils/util.h"

/**
* @brief
*     Initialize the object.
*/
scan::SvcTable::SvcTable(SvcTable&& t_table) noexcept : SvcTable{}
{
    m_addr = t_table.m_addr;
    m_argsp = std::move(t_table.m_argsp);
    m_list = t_table.m_list;
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcTable::SvcTable(const string& t_addr,
                         shared_ptr<Args> t_argsp,
                         const vector<value_type>& t_vect)
    : SvcTable{}
{
    m_addr = t_addr;
    m_argsp = t_argsp;

    add(t_vect);
    sort();
}

/**
* @brief
*     Get the underlying service information as a string.
*     Optionally includes the underlying HTTP responses.
*/
std::string scan::SvcTable::str(const bool& t_colorize) const
{
    return algo::concat(table_str(t_colorize), LF, details_str(t_colorize));
}

/**
* @brief
*     Get the underlying service information table as a string.
*/
std::string scan::SvcTable::table_str(const bool& t_colorize) const
{
    sstream stream;

    // Add scan table title
    if (!m_addr.empty())
    {
        stream << util::header_title("Target", m_addr, t_colorize) << LF;
    }
    const size_map size_map{make_size_map()};

    // Add header table record
    const string_vector header_fields
    {
        algo::pad("PORT", size_map.at(field_t::port)),
        algo::pad("SERVICE", size_map.at(field_t::service)),
        algo::pad("STATE", size_map.at(field_t::state)),
        algo::pad("INFO", size_map.at(field_t::summary))
    };
    const string delim{"   "};

    const string header{algo::join(header_fields, delim)};
    stream << (t_colorize ? util::colorize(header, Color::green) : header) << LF;

    // Add populated table records
    for (const value_type& svc_info : m_list)
    {
        const string_vector record_fields
        {
            algo::pad(svc_info.port_str(), size_map.at(field_t::port)),
            algo::pad(svc_info.service, size_map.at(field_t::service)),
            algo::pad(svc_info.state_str(), size_map.at(field_t::state)),
            algo::pad(svc_info.summary, size_map.at(field_t::summary))
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
        throw RuntimeEx{"SvcTable::args", "Underlying arguments pointer is null"};
    }
    return *m_argsp;
}

/**
* @brief
*     Create a mapping of the underlying service
*     information fields and their maximum sizes.
*/
scan::SvcTable::size_map scan::SvcTable::make_size_map() const
{
    return size_map
    {
        {field_t::port,    max_field_size(field_t::port)},
        {field_t::state,   max_field_size(field_t::state)},
        {field_t::service, max_field_size(field_t::service)},
        {field_t::summary, max_field_size(field_t::summary)}
    };
}

/**
* @brief
*     Get the details about the underlying services as a string.
*/
std::string scan::SvcTable::details_str(const bool& t_colorize) const
{
    sstream stream;

    for (const value_type& info : m_list)
    {
        stream << info.details(t_colorize);

        if (&info != &m_list.last())
        {
            stream << LF;
        }
    }
    return stream.str();
}
