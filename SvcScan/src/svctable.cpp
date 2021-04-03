/*
*  svctable.cpp
*  ------------
*  Source file for a two dimensional container
*/
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
        m_list += Record({"PORT", "STATE", "SERVICE", "VERSION"});
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
    m_list += Record(si);
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
    // Map for table field (max) widths
    const map_sf<size_t> width_map({
        {SvcField::port,    max_width(SvcField::port)},
        {SvcField::state,   max_width(SvcField::state)},
        {SvcField::service, max_width(SvcField::service)},
        {SvcField::version, max_width(SvcField::version)}
    });

    size_t pos = {0};
    std::stringstream ss;

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title(Util::fmt("Target: %", m_addr));

        ss << title << Util::LF
            << string(title.size(), '-') << Util::LF;
    }

    /**
    *  TODO: sort records in table by port number
    **/

    // Pad and add rows to new list
    for (const Record &row : m_list)
    {
        ss << list_r::join({row.pad_fields(width_map)}, "  ") << Util::LF;
        pos++;
    }
    return ss.str();
}

/// ***
/// Get the max character width of the given field
/// ***
const int Scan::SvcTable::max_width(const SvcField &sf) const
{
    size_t max_width = {0};

    // Compare fields width to previous max
    for (const Record &row : m_list)
    {
        const size_t width = {row[sf].size()};
        max_width = (width > max_width) ? width : max_width;
    }
    return static_cast<int>(max_width);
}
