/*
*  svctable.cpp
*  ------------
*  Source file for a two dimensional container
*/
#include <algorithm>
#include "includes/containers/svctable.h"

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
    sstream ss;
    vector_r vect{ m_list };

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title{ Util::fstr("Target: %", m_addr) };

        ss << title << stdu::LF
            << string(title.size(), '-') << stdu::LF;
    }

    std::sort(vect.begin() + 1, vect.end(), Record::is_less_predicate);
    vector_r::const_iterator begin_it{ vect.cbegin() + 1 };

    // Determine whether summary field should be hidden
    Record::hide_sum = std::all_of(begin_it, vect.cend(), [](const Record &l_rec)
    {
        return l_rec.summary.empty();
    });

    const string delim{ Record::hide_sum ? "    " : "   " };

    // Map for table field (max) widths
    const field_map<size_t> width_map
    {
        { field::port,    max_width(vect, field::port) },
        { field::state,   max_width(vect, field::state) },
        { field::service, max_width(vect, field::service) },
        { field::info,    max_width(vect, field::info) }
    };

    // Pad/add record strings to string stream
    for (const Record &rec : vect)
    {
        const string row_str{ list_r::join({ rec.pad_fields(width_map) }, delim) };

        // Hide summary field header substring
        if (Record::hide_sum && (rec == *vect.cbegin()))
        {
            ss << row_str.substr(0, (row_str.find("SERVICE") + 7)) << stdu::LF;
            continue;
        }
        ss << row_str << stdu::LF;
    }
    return ss.str();
}

/// ***
/// Get the max character width of the given field
/// ***
size_t scan::SvcTable::max_width(const vector_r &t_vect, const field &t_sf) const
{
    size_t max_width{ 0 };

    // Compare field width to previous max
    for (const Record &record : t_vect)
    {
        const size_t width{ record[t_sf].size() };
        max_width = (width > max_width) ? width : max_width;
    }
    return max_width;
}
