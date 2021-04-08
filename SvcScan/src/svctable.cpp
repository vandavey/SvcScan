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
Scan::SvcTable::SvcTable(const SvcTable &st) : SvcTable()
{
    this->m_list = st.m_list;
}

/// ***
/// Initialize the object
/// ***
Scan::SvcTable::SvcTable(const string &addr, const vector_si &vect) : SvcTable()
{
    this->m_addr = addr;
    this->add(vect);
}

/// ***
/// Initialize the object
/// ***
Scan::SvcTable::SvcTable()
{
    // Add header record
    if (m_list.empty())
    {
        m_list.add(Record({"PORT", "STATE", "SERVICE", "VERSION"}));
    }
}

/// ***
/// Subscript operator overload
/// ***
const Scan::Record Scan::SvcTable::operator[](const size_t &index) const
{
    if (index >= m_list.size())
    {
        throw ArgEx("index", "Invalid list index (out of range)");
    }
    return m_list.at(index);
}

/// ***
/// Add a new record to the underlying list
/// ***
void Scan::SvcTable::add(const SvcInfo &si)
{
    m_list.add(Record(si));
}

/// ***
/// Add new records to the underlying list
/// ***
void Scan::SvcTable::add(const vector_si &vect)
{
    for (const SvcInfo &si : vect)
    {
        add(si);
    }
}

/// ***
/// Format the underlying list as a table string
/// ***
const std::string Scan::SvcTable::str() const
{
    std::stringstream ss;

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title(Util::fmt("Target: %", m_addr));

        ss << title << Util::LF
            << string(title.size(), '-') << Util::LF;
    }
    vector_r vect(m_list);

    // Sort by port number (ascending)
    std::sort(vect.begin() + 1, vect.end(), Record::is_less);

    // Map for table field (max) widths
    const map_sf<size_t> width_map
    {
        {SvcField::port,    field_width(vect, SvcField::port)},
        {SvcField::state,   field_width(vect, SvcField::state)},
        {SvcField::service, field_width(vect, SvcField::service)},
        {SvcField::version, field_width(vect, SvcField::version)}
    };

    // Pad and add rows to new list
    for (const Record &row : vect)
    {
        ss << list_r::join({row.pad_fields(width_map)}, "  ") << Util::LF;
    }
    return ss.str();
}

/// ***
/// Get the max character width of the given field
/// ***
const int Scan::SvcTable::field_width(const vector_r &vect, 
                                      const SvcField &sf) const {
    size_t max_width = {0};

    // Compare fields width to previous max
    for (const Record &row : m_list)
    {
        const size_t width = {row.get_field(sf).size()};
        max_width = (width > max_width) ? width : max_width;
    }
    return static_cast<int>(max_width);
}
