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
scan::SvcTable::SvcTable(const string &t_addr, const vector_si &t_vect) : SvcTable()
{
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
        m_list.add(Record{ "PORT", "STATE", "SERVICE", "INFO" });
    }
}

/// ***
/// Add a new record to the underlying list
/// ***
void scan::SvcTable::add(const SvcInfo &t_si)
{
    m_list.add(Record{ t_si });
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
std::string scan::SvcTable::str() const
{
    std::stringstream ss;
    vector_r vect{ m_list };

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title{ Util::fstr("Target: %", m_addr) };

        ss << title << Util::LF
            << string(title.size(), '-') << Util::LF;
    }

    std::sort(vect.begin() + 1, vect.end(), Record::is_less);
    vector_r::const_iterator begin_pos{ vect.cbegin() + 1 };

    // Determine if info field should be hidden
    Record::hide_info = std::all_of(begin_pos, vect.cend(), [](const Record &t_rec)
    {
        return t_rec.info.get().empty();
    });

    const string delim{ Record::hide_info ? "    " : "   " };

    // Map for table field (max) widths
    const map_sf<size_t> width_map
    {
        { field::port,    get_width(vect, field::port) },
        { field::state,   get_width(vect, field::state) },
        { field::service, get_width(vect, field::service) },
        { field::info,    get_width(vect, field::info) }
    };

    // Pad/add record strings to string stream
    for (const Record &rec : vect)
    {
        const string row_str{ list_r::join({ rec.pad_fields(width_map) }, delim) };

        // Hide info field header substring
        if (Record::hide_info && (rec == *vect.cbegin()))
        {
            ss << row_str.substr(0, (row_str.find("SERVICE") + 7)) << Util::LF;
            continue;
        }
        ss << row_str << Util::LF;
    }
    return ss.str();
}

/// ***
/// Get the max character width of the given field
/// ***
size_t scan::SvcTable::get_width(const vector_r &t_vect, const field &t_sf) const
{
    size_t max_width{ 0 };

    // Compare fields width to previous max
    for (const Record &rec : t_vect)
    {
        const size_t width{ rec.get_field(t_sf).size() };
        max_width = (width > max_width) ? width : max_width;
    }
    return max_width;
}
