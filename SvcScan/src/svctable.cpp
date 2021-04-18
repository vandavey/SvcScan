/*
*  svctable.cpp
*  ------------
*  Source file for a two dimensional container
*/
#include <algorithm>
#include "includes/container/svctable.h"

/// ***
/// Initialize the object
/// ***
scan::SvcTable::SvcTable(const SvcTable &t_st) : SvcTable()
{
    m_list = t_st.m_list;
}

/// ***
/// Initialize the object
/// ***
scan::SvcTable::SvcTable(const string &t_addr,
                         const vector_si &t_vect) : SvcTable() {
    m_addr = t_addr;
    add(t_vect);
}

/// ***
/// Initialize the object
/// ***
scan::SvcTable::SvcTable()
{
    // Add header record
    if (m_list.empty())
    {
        m_list.add(Record({ "PORT", "STATE", "SERVICE", "VERSION" }));
    }
}

/// ***
/// Subscript operator overload
/// ***
const scan::Record scan::SvcTable::operator[](const size_t &t_idx) const
{
    if (t_idx >= m_list.size())
    {
        throw ArgEx("t_idx", "Invalid list index (out of range)");
    }
    return m_list.at(t_idx);
}

/// ***
/// Add a new record to the underlying list
/// ***
void scan::SvcTable::add(const SvcInfo &t_si)
{
    m_list.add(Record(t_si));
}

/// ***
/// Add new records to the underlying list
/// ***
void scan::SvcTable::add(const vector_si &t_vect)
{
    for (const SvcInfo &si : t_vect)
    {
        add(si);
    }
}

/// ***
/// Format the underlying list as a table string
/// ***
const std::string scan::SvcTable::str() const
{
    std::stringstream ss;

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title{ Util::fmt("Target: %", m_addr) };

        ss << title << Util::LF
            << string(title.size(), '-') << Util::LF;
    }
    vector_r vect{ m_list };

    // Sort by port number (ascending)
    std::sort(vect.begin() + 1, vect.end(), Record::is_less);

    // Map for table field (max) widths
    const map_sf<size_t> width_map
    {
        { field::port,    field_width(vect, field::port) },
        { field::state,   field_width(vect, field::state) },
        { field::service, field_width(vect, field::service) },
        { field::version, field_width(vect, field::version) }
    };

    // Pad and add rows to new list
    for (const Record &row : vect)
    {
        ss << list_r::join({ row.pad_fields(width_map) }, "  ") << Util::LF;
    }
    return ss.str();
}

/// ***
/// Get the max character width of the given field
/// ***
const int scan::SvcTable::field_width(const vector_r &t_vect, 
                                      const field &t_sf) const {
    size_t max_width{ 0 };

    // Compare fields width to previous max
    for (const Record &row : t_vect)
    {
        const size_t width{ row.get_field(t_sf).size() };
        max_width = (width > max_width) ? width : max_width;
    }
    return static_cast<int>(max_width);
}
