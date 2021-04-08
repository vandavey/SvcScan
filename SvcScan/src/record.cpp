/*
*  record.cpp
*  ----------
*  Source file for a table record
*/
#include "includes/container/record.h"

/// ***
/// Initialize the object
/// ***
Scan::Record::Record(const Record &row)
{
    this->port = row.port;
    this->state = row.state;
    this->service = row.service;
    this->version = row.version;
}

/// ***
/// Initialize the object
/// ***
Scan::Record::Record(const array_s &fields)
{
    this->port = fields[0];
    this->state = fields[1];
    this->service = fields[2];
    this->version = fields[3];
}

/// ***
/// Initialize the object
/// ***
Scan::Record::Record(const SvcInfo &si)
{
    this->port = si.port.get() + "/tcp";
    this->state = state_str(si.state.get());
    this->service = si.service.get();
    this->version = si.version.get();
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
/// Determine if the left-hand record's port number is less
/// ***
const bool Scan::Record::is_less(const Record &lhs, const Record &rhs)
{
    return stoi(lhs.port.get()) < stoi(rhs.port.get());
}

/// ***
/// Retrieve the value associated with the given field
/// ***
const std::string Scan::Record::get_field(const SvcField &sf) const
{
    switch (sf)
    {
        case SvcField::port:
            return port.get();
        case SvcField::service:
            return service.get();
        case SvcField::state:
            return state.get();
        case SvcField::version:
            return version.get();
        default:
            return "";
    }
}

/// ***
/// Set the value associated with the given field
/// ***
void Scan::Record::set_field(const SvcField &sf, const string &value)
{
    switch (sf)
    {
        case SvcField::port:
            port = value;
            break;
        case SvcField::service:
            service = value;
            break;
        case SvcField::state:
            state = value;
            break;
        case SvcField::version:
            version = value;
            break;
        default:
            break;
    }
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
        const size_t field_width = {get_field(pair.first).size()};

        // Invalid maximum width
        if (pair.second < field_width)
        {
            throw ArgEx("width_map", "Invalid key value (size_t)");
        }
        const size_t delta = {pair.second - field_width};

        // Append padding to field
        if (delta > 0)
        {
            const string value(get_field(pair.first) + string(delta, ' '));
            clone.set_field(pair.first, value);
        }
    }
    return clone;
}

/// ***
/// Get the string equivalent of the given host state
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
