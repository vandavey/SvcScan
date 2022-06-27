/*
*  svc_table.cpp
*  -------------
*  Source file for a network application service table
*/
#include "includes/containers/svc_table.h"

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable(const SvcTable &t_st) : SvcTable()
{
    m_list = t_st.m_list;
}

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable(const string &t_addr, const vector<SvcInfo> &t_vect)
    : SvcTable() {

    m_addr = t_addr;
    add(t_vect);
}

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable()
{
    // Add header record
    if (m_list.empty())
    {
        m_list.add(Record{ "PORT", "STATE", "SERVICE", "INFO" });
    }
}

/**
* @brief  Add a new record to the underlying list of service information.
*/
void scan::SvcTable::add(const SvcInfo &t_si)
{
    m_list.add(Record{ t_si });
}

/**
* @brief  Add new records to the underlying list of service information.
*/
void scan::SvcTable::add(const vector<SvcInfo> &t_vect)
{
    for (const SvcInfo &si : t_vect)
    {
        add(si);
    }
}

/**
* @brief  Get the underlying service information table as a string.
*/
std::string scan::SvcTable::str() const
{
    sstream ss;
    vector<Record> vect{ m_list };

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title{ Util::fstr("Target: %", m_addr) };

        ss << title << stdu::LF
            << string(title.size(), '-') << stdu::LF;
    }

    std::sort(vect.begin() + 1, vect.end(), Record::is_less_predicate);
    vector<Record>::const_iterator begin_it{ vect.cbegin() + 1 };

    // Determine whether summary field should be hidden
    Record::hide_sum = std::all_of(begin_it, vect.cend(), [](const Record &l_rec)
    {
        return l_rec.summary.empty();
    });
    const string sep{ Record::hide_sum ? "    " : "   " };

    // Map for table field (max) widths
    const field_map<size_t> width_map
    {
        { field::port,    max_width(vect, field::port) },
        { field::state,   max_width(vect, field::state) },
        { field::service, max_width(vect, field::service) },
        { field::info,    max_width(vect, field::info) }
    };

    // Pad and add insert record into the stream
    for (const Record &rec : vect)
    {
        const string row_str = List<Record>({ rec.pad_fields(width_map) }).join(sep);

        // Hide summary field header
        if (Record::hide_sum && (rec == *vect.cbegin()))
        {
            ss << row_str.substr(0, (row_str.find("SERVICE") + 7)) << stdu::LF;
            continue;
        }
        ss << row_str << stdu::LF;
    }
    return ss.str();
}

/**
* @brief  Get the max character width of the given service record field.
*/
size_t scan::SvcTable::max_width(const vector<Record> &t_vect,
                                 const field &t_sf) const {
    size_t max_width{ 0 };

    // Compare field width to previous max
    for (const Record &record : t_vect)
    {
        const size_t width{ record[t_sf].size() };
        max_width = (width > max_width) ? width : max_width;
    }
    return max_width;
}
