/*
*  svc_table.cpp
*  -------------
*  Source file for a network application service table
*/
#include "includes/containers/svc_table.h"

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
    for (const SvcInfo &info : t_vect)
    {
        add(info);
    }
}

/**
* @brief  Sort the underlying service list by port number (excluding header record).
*/
void scan::SvcTable::sort()
{
    vector<value_type> vect{ m_list };

    ranges::sort(vector<value_type>::iterator{ vect.begin() + 1 },
                 vect.end(),
                 ranges::less(),
                 &value_type::get_port);

    m_list = vect;
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
    std::stringstream stream;
    vector<value_type> vect{ m_list };

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title{ algo::fstr("Target: %", m_addr) };

        stream << title                     << stdu::LF
               << string(title.size(), '-') << stdu::LF;
    }

    auto condition = [](const value_type &l_info) -> bool
    {
        return l_info.summary.empty();
    };

    value_type::no_summary = std::all_of(vect.cbegin() + 1, vect.cend(), condition);
    const string sep{ value_type::no_summary ? "    " : "   " };

    const field_map width_map
    {
        { field_t::port,    max_width(vect, field_t::port) },
        { field_t::state,   max_width(vect, field_t::state) },
        { field_t::service, max_width(vect, field_t::service) },
        { field_t::summary, max_width(vect, field_t::summary) }
    };

    // Pad and add insert record into the stream
    for (const value_type &info : vect)
    {
        const string row_str{ algo::join(info.pad_fields(width_map), sep) };

        // Hide summary field header
        if (value_type::no_summary && info == *vect.cbegin())
        {
            stream << row_str.substr(0, row_str.find("SERVICE") + 7) << stdu::LF;
            continue;
        }
        stream << row_str << stdu::LF;
    }
    return stream.str();
}

/**
* @brief  Get the max character width of the given service record field.
*/
size_t scan::SvcTable::max_width(const vector<value_type> &t_vect,
                                 const field_t &t_field) const {
    size_t max_width{ 0 };

    for (const value_type &info : t_vect)
    {
        const size_t width{ info[t_field].size() };
        max_width = width > max_width ? width : max_width;
    }
    return max_width;
}
