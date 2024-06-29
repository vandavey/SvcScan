/*
* @file
*     svc_table.cpp
* @brief
*     Source file for a network application service table.
*/
#include <algorithm>
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
scan::SvcTable::SvcTable()
{
    if (m_list.empty())
    {
        m_list.add(value_type{ "PORT", "STATE", "SERVICE", "INFO", true });
    }
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcTable::SvcTable(SvcTable &&t_table) noexcept : SvcTable()
{
    m_argsp = std::move(t_table.m_argsp);
    m_list = t_table.m_list;
}

/**
* @brief
*     Initialize the object.
*/
scan::SvcTable::SvcTable(const string &t_addr,
                         shared_ptr<Args> t_argsp,
                         const vector<value_type> &t_vect)
    : SvcTable()
{
    m_addr = t_addr;
    m_argsp = t_argsp;

    add(t_vect);
    sort();
}

/**
* @brief
*     Add a new record to the underlying list of service information.
*/
void scan::SvcTable::add(const value_type &t_info)
{
    m_list.add(t_info);
}

/**
* @brief
*     Add new records to the underlying list of service information.
*/
void scan::SvcTable::add(const vector<value_type> &t_vect)
{
    m_list.add_range(t_vect);
}

/**
* @brief
*     Sort the underlying service list by port number (excluding header record).
*/
void scan::SvcTable::sort()
{
    vector<value_type> &vect{ m_list.vector() };
    ranges::sort(vect.begin() + 1, vect.end(), ranges::less(), &value_type::port);
}

/**
* @brief
*     Determine whether the underlying service information list
*     is empty. Optionally includes the table header element.
*/
bool scan::SvcTable::empty(const bool &t_inc_header) const noexcept
{
    return size(t_inc_header) == 0;
}

/**
* @brief
*     Get the size of the underlying service information
*     list. Optionally includes the table header element.
*/
size_t scan::SvcTable::size(const bool &t_inc_header) const noexcept
{
    return t_inc_header || m_list.empty() ? m_list.size() : m_list.size() - 1;
}

/**
* @brief
*     Get a constant iterator to the first element in the underlying list.
*/
scan::SvcTable::iterator scan::SvcTable::begin() const noexcept
{
    return m_list.begin();
}

/**
* @brief
*     Get a constant iterator to the past-the-end element in the underlying list.
*/
scan::SvcTable::iterator scan::SvcTable::end() const noexcept
{
    return m_list.end();
}

/**
* @brief
*     Get a constant reference to the underlying target hostname or IPv4 address.
*/
const std::string &scan::SvcTable::addr() const noexcept
{
    return m_addr;
}

/**
* @brief
*     Get the underlying service information as a string.
*     Optionally includes the underlying HTTP responses.
*/
std::string scan::SvcTable::str(const bool &t_colorize) const
{
    return algo::concat(table_str(t_colorize), LF, details_str(t_colorize));
}

/**
* @brief
*     Get the underlying service information table as a string.
*/
std::string scan::SvcTable::table_str(const bool &t_colorize) const
{
    sstream stream;

    // Add scan table title
    if (!m_addr.empty())
    {
        stream << util::header_title("Target", m_addr, t_colorize) << LF;
    }
    const List<value_type> info_list{ data() };

    value_type::no_summary = ranges::all_of(info_list, [](const value_type &l_info)
    {
        return l_info.summary.empty();
    });
    const field_map width_map{ make_width_map() };

    const string col_delim{ value_type::no_summary ? "    " : "   " };
    string header{ algo::join(begin()->pad_fields(width_map), col_delim) };

    // Hide the summary field header
    if (value_type::no_summary)
    {
        header = header.substr(0U, header.find("SERVICE") + 7U);
    }

    stream << (t_colorize ? util::colorize(header, Color::green) : header) << LF;

    // Pad service fields and add write record to stream
    for (const value_type &info : info_list)
    {
        stream << algo::join(info.pad_fields(width_map), col_delim) << LF;
    }
    return stream.str();
}

/**
* @brief
*     Get a constant reference to the underlying command-line arguments.
*/
const scan::Args &scan::SvcTable::args() const
{
    if (m_argsp == nullptr)
    {
        throw RuntimeEx{ "SvcTable::args", "Underlying arguments pointer is null" };
    }
    return *m_argsp;
}

/**
* @brief
*     Create a mapping of the underlying service
*     information fields and their maximum sizes.
*/
scan::SvcTable::field_map scan::SvcTable::make_width_map() const
{
    return field_map
    {
        { field_t::port,    max_width(field_t::port) },
        { field_t::state,   max_width(field_t::state) },
        { field_t::service, max_width(field_t::service) },
        { field_t::summary, max_width(field_t::summary) }
    };
}

/**
* @brief
*     Get a copy of the underlying service information
*     list without the field headers included.
*/
scan::List<scan::SvcInfo> scan::SvcTable::data() const
{
    return m_list.slice(1U);
}

/**
* @brief
*     Get the max character width of the given service record field.
*/
size_t scan::SvcTable::max_width(const field_t &t_field) const
{
    size_t max_width{ 0U };

    for (const value_type &info : m_list)
    {
        const size_t width{ info[t_field].size() };
        max_width = width > max_width ? width : max_width;
    }
    return max_width;
}

/**
* @brief
*     Get the details about the underlying services as a string.
*/
std::string scan::SvcTable::details_str(const bool &t_colorize) const
{
    sstream stream;
    const List<value_type> info_list{ data() };

    for (const value_type &info : info_list)
    {
        stream << info.details(t_colorize);

        if (&info != &info_list.last())
        {
            stream << LF;
        }
    }
    return stream.str();
}
