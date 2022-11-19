/*
*  svc_table.cpp
*  -------------
*  Source file for a network application service table
*/
#include "includes/containers/svc_table.h"

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable()
{
    if (m_list.empty())
    {
        m_list.add(value_type{ "PORT", "STATE", "SERVICE", "INFO", true });
    }
}

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable(const SvcTable &t_table) : this_t()
{
    m_list = t_table.m_list;
}

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable(const string &t_addr, const vector<value_type> &t_vect)
    : this_t() {

    m_addr = t_addr;

    add(t_vect);
    sort();
}

/**
* @brief  Add a new record to the underlying list of service information.
*/
void scan::SvcTable::add(const value_type &t_info)
{
    m_list.add(t_info);
}

/**
* @brief  Add new records to the underlying list of service information.
*/
void scan::SvcTable::add(const vector<value_type> &t_vect)
{
    m_list.add_range(t_vect);
}

/**
* @brief  Sort the underlying service list by port number (excluding header record).
*/
void scan::SvcTable::sort()
{
    vector<value_type> &vect{ m_list.vector() };
    ranges::sort(vect.begin() + 1, vect.end(), ranges::less(), &value_type::port);
}

/**
* @brief  Determine whether the underlying service information list is empty.
*         Optionally includes the table header element.
*/
bool scan::SvcTable::empty(const bool &t_inc_header) const noexcept
{
    return size(t_inc_header) == 0;
}

/**
* @brief  Get the size of the underlying service information list. Optionally
*         includes the table header element.
*/
size_t scan::SvcTable::size(const bool &t_inc_header) const noexcept
{
    return t_inc_header || m_list.empty() ? m_list.size() : m_list.size() - 1;
}

/**
* @brief  Get a constant pointer to the array of the underlying list.
*/
const scan::SvcInfo *scan::SvcTable::data() const noexcept
{
    return m_list.data();
}

/**
* @brief  Get a pointer to the array of the underlying list.
*/
scan::SvcInfo *scan::SvcTable::data() noexcept
{
    return m_list.data();
}

/**
* @brief  Get a constant iterator to the first element in the underlying list.
*/
scan::SvcTable::iterator scan::SvcTable::begin() const noexcept
{
    return m_list.begin();
}

/**
* @brief  Get a constant iterator to the past-the-end element in
*         the underlying list. 
*/
scan::SvcTable::iterator scan::SvcTable::end() const noexcept
{
    return m_list.end();
}

/**
* @brief  Get a constant reference to the underlying target hostname or IPv4 address.
*/
const std::string &scan::SvcTable::addr() const noexcept
{
    return m_addr;
}

/**
* @brief  Get the underlying service information table as a string.
*/
std::string scan::SvcTable::str() const
{
    sstream stream;

    // Add scan table title
    if (!m_addr.empty())
    {
        stream << algo::underline(algo::fstr("Target: %", m_addr), '=') << &LF[0];
    }

    auto all_pred = [](const value_type &l_info) -> bool
    {
        return l_info.summary.empty();
    };
    const vector<value_type> &vect{ m_list.vector() };

    value_type::no_summary = ranges::all_of(vect.begin() + 1, vect.end(), all_pred);
    const string sep{ value_type::no_summary ? "    " : "   " };

    const field_map width_map
    {
        { field_t::port,    max_width(field_t::port) },
        { field_t::state,   max_width(field_t::state) },
        { field_t::service, max_width(field_t::service) },
        { field_t::summary, max_width(field_t::summary) }
    };

    // Pad service fields and add write record to stream
    for (const value_type &info : vect)
    {
        const string row_str{ algo::join(info.pad_fields(width_map), sep) };

        // Hide the summary header field
        if (value_type::no_summary && info == *vect.begin())
        {
            stream << row_str.substr(0, row_str.find("SERVICE") + 7) << &LF[0];
            continue;
        }
        stream << row_str << &LF[0];
    }
    return stream.str();
}

/**
* @brief  Get the max character width of the given service record field.
*/
size_t scan::SvcTable::max_width(const field_t &t_field) const
{
    size_t max_width{ 0 };

    for (const value_type &info : m_list)
    {
        const size_t width{ info[t_field].size() };
        max_width = width > max_width ? width : max_width;
    }
    return max_width;
}
