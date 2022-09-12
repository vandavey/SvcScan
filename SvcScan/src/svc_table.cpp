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
scan::SvcTable::SvcTable(const string &t_addr, const vector<SvcInfo> &t_vect)
    : this_t() {

    m_addr = t_addr;
    add(t_vect);
}

/**
* @brief  Initialize the object.
*/
scan::SvcTable::SvcTable()
{
    if (m_list.empty())
    {
        m_list.add(Record{ "PORT", "STATE", "SERVICE", "INFO" });
    }
}

/**
* @brief  Add a new record to the underlying list of service information.
*/
void scan::SvcTable::add(const SvcInfo &t_info)
{
    m_list.add(Record{ t_info });
}

/**
* @brief  Add new records to the underlying list of service information.
*/
void scan::SvcTable::add(const vector<SvcInfo> &t_vect)
{
    for (const SvcInfo &info : t_vect)
    {
        add(info);
    }
}

/**
* @brief  Get a constant pointer to the array of the underlying list.
*/
const scan::Record *scan::SvcTable::data() const noexcept
{
    return m_list.data();
}

/**
* @brief  Get a pointer to the array of the underlying list.
*/
scan::Record *scan::SvcTable::data() noexcept
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
    vector<Record> vect{ m_list };

    // Add scan table title
    if (!m_addr.empty())
    {
        const string title{ algo::fstr("Target: %", m_addr) };

        stream << title                     << stdu::LF
               << string(title.size(), '-') << stdu::LF;
    }

    ranges::sort(vect.begin() + 1, vect.end(), ranges::less(), &Record::port_num);
    vector<Record>::const_iterator begin_it{ vect.cbegin() + 1 };

    // Determine whether summary field should be hidden
    Record::hide_sum = std::all_of(begin_it, vect.cend(), [](const Record &l_rec)
    {
        return l_rec.summary.empty();
    });
    const string sep{ Record::hide_sum ? "    " : "   " };

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
        const string row_str{ algo::join(rec.pad_fields(width_map), sep) };

        // Hide summary field header
        if (Record::hide_sum && rec == *vect.cbegin())
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
size_t scan::SvcTable::max_width(const vector<Record> &t_vect,
                                 const field &t_field) const {
    size_t max_width{ 0 };

    // Compare field width to previous max
    for (const Record &record : t_vect)
    {
        const size_t width{ record[t_field].size() };
        max_width = width > max_width ? width : max_width;
    }
    return max_width;
}
