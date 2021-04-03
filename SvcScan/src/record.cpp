/*
*  record.cpp
*  ----------
*  Source file for a table record
*/
#include "includes/container/record.h"

/// ***
/// Initialize the object
/// ***
Scan::Record::Record()
{
    this->swap(*this);
}

/// ***
/// Initialize the object
/// ***
Scan::Record::Record(const Record &row) : Record()
{
    this->swap(row);
}

/// ***
/// Initialize the object
/// ***
Scan::Record::Record(const array_s &fields) : Record()
{
    this->m_port = fields[0];
    this->m_state = fields[1];
    this->m_service = fields[2];
    this->m_version = fields[3];

    this->swap(*this);
}

/// ***
/// Initialize the object
/// ***
Scan::Record::Record(const SvcInfo &si) : Record()
{
    this->m_port = si.port.get() + "/tcp";
    this->m_state = state_str(si.state.get());
    this->m_service = si.service.get();
    this->m_version = si.version.get();

    this->swap(*this);
}

/// ***
/// Cast operator overload
/// ***
Scan::Record::operator const array_s() const
{
    return {port.get(), state.get(), service.get(), version.get()};
}

/// ***
/// Cast operator overload
/// ***
Scan::Record::operator const std::string() const
{
    return list_s::join(operator const vector_s(), "  ");
}

/// ***
/// Cast operator overload
/// ***
Scan::Record::operator const vector_s() const
{
    return {port.get(), state.get(), service.get(), version.get()};
}

/// ***
/// Equality operator overload
/// ***
const bool Scan::Record::operator==(const Record &row) const
{
    return operator const array_s() == static_cast<array_s>(row);
}

/// ***
/// Inequality operator overload
/// ***
const bool Scan::Record::operator!=(const Record &row) const
{
    return operator const array_s() != static_cast<array_s>(row);
}

/// ***
/// Subscript operator overload
/// ***
const std::string &Scan::Record::operator[](const SvcField &sf) const
{
    return m_map.at(sf);
}

/// ***
/// Subscript operator overload
/// ***
std::string &Scan::Record::operator[](const SvcField &sf)
{
    return m_map.at(sf);
}

/// ***
/// Add padding to all fields in vector and return as copy
/// ***
const Scan::Record Scan::Record::pad_fields(const map_sf<size_t> &dict) const
{
    Record clone(*this);

    // Add padding to fields
    for (const map_sf<size_t>::value_type &pair : dict)
    {
        const size_t field_width = {(operator[](pair.first)).size()};

        // Invalid maximum width
        if (pair.second < field_width)
        {
            throw ArgEx("width_map", "Invalid key value (size_t)");
        }
        const size_t delta = {pair.second - field_width};

        // Append padding to field
        if (delta > 0)
        {
            clone[pair.first] += string(delta, ' ');
        }
    }
    return clone;
}

/// ***
/// Get the string equivalent of the given port state
/// ***
const std::string Scan::Record::state_str(const HostState &hs) const noexcept
{
    switch (hs)
    {
        case HostState::open:    // Open state
        {
            return "open";
        }
        case HostState::closed:  // Closed state
        {
            return "closed";
        }
        default:  // Unknown state
        {
            return "unknown";
        }
    }
}

/// ***
/// Swap mutable member values with reference object values
/// ***
Scan::Record &Scan::Record::swap(const Record &row) noexcept
{
    m_port = row.port.get();
    m_state = row.state.get();
    m_service = row.service.get();
    m_version = row.version.get();

    port = &m_port;
    state = &m_state;
    service = &m_service;
    version = &m_version;

    // Map field enum types to field references
    m_map = {
        {SvcField::port,    static_cast<string &>(m_port)},
        {SvcField::state,   static_cast<string &>(m_state)},
        {SvcField::service, static_cast<string &>(m_service)},
        {SvcField::version, static_cast<string &>(m_version)}
    };
    return *this;
}
