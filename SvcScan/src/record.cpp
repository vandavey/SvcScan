/*
*  record.cpp
*  ----------
*  Source file for a table record
*/
#include "includes/container/record.h"

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const Record &t_row)
{
    port = t_row.port;
    state = t_row.state;
    service = t_row.service;
    version = t_row.version;
}

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const il_s &t_il)
{
    // Invalid argument list size
    if (t_il.size() != 4)
    {
        throw ArgEx("t_il", "Invalid size for initializer list");
    }
    operator=(List<string>::copy_n<4>(t_il));
}

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const SvcInfo &t_si)
{
    port = t_si.port + "/tcp";
    state = state_str(t_si.state);
    service = t_si.service;
    version = t_si.version;
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator array_s() const
{
    return { port, state, service, version };
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator string() const
{
    return list_s::join(operator vector_s(), "  ");
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator vector_s() const
{
    return { port, state, service, version };
}

/// ***
/// Assignment operator overload
/// ***
scan::Record &scan::Record::operator=(const array_s &t_fields)
{
    port = t_fields[0];
    state = t_fields[1];
    service = t_fields[2];
    version = t_fields[3];

    return *this;
}

/// ***
/// Equality operator overload
/// ***
bool scan::Record::operator==(const Record &t_row) const
{
    return operator array_s() == static_cast<array_s>(t_row);
}

/// ***
/// Inequality operator overload
/// ***
bool scan::Record::operator!=(const Record &t_row) const
{
    return operator array_s() != static_cast<array_s>(t_row);
}

/// ***
/// Determine if the left-hand record's port number is less
/// ***
bool scan::Record::is_less(const Record &t_lhs, const Record &t_rhs)
{
    return std::stoi(t_lhs.port) < std::stoi(t_rhs.port);
}

/// ***
/// Retrieve the value associated with the given field
/// ***
std::string scan::Record::get_field(const field &t_sf) const
{
    switch (t_sf)
    {
        case field::port:     // Port number
            return port;
        case field::service:  // Service name
            return service;
        case field::state:    // Target state
            return state;
        case field::version:  // Service version
            return version;
        default:
            return string();
    }
}

/// ***
/// Set the value associated with the given field
/// ***
void scan::Record::set_field(const field &t_sf, const string &t_value)
{
    switch (t_sf)
    {
        case field::port:     // Port number
            port = t_value;
            break;
        case field::service:  // Service name
            service = t_value;
            break;
        case field::state:    // Target state
            state = t_value;
            break;
        case field::version:  // Service version
            version = t_value;
            break;
        default:
            break;
    }
}

/// ***
/// Add padding to all fields in vector and return as copy
/// ***
scan::Record scan::Record::pad_fields(const map_sf<size_t> &t_dict) const
{
    Record clone{ *this };

    // Add padding to fields
    for (const map_sf<size_t>::value_type &pair : t_dict)
    {
        const size_t field_width{ get_field(pair.first).size() };

        // Invalid maximum width
        if (pair.second < field_width)
        {
            throw ArgEx("t_dict", "Invalid key value (size_t)");
        }
        const size_t delta{ pair.second - field_width };

        // Append padding to field
        if (delta > 0)
        {
            const string value{ get_field(pair.first) + string(delta, ' ') };
            clone.set_field(pair.first, value);
        }
    }
    return clone;
}

/// ***
/// Get the string equivalent of the given host state
/// ***
std::string scan::Record::state_str(const HostState &t_hs) const noexcept
{
    switch (t_hs)
    {
        case HostState::open:    // Open
            return "open";
        case HostState::closed:  // Closed
            return "closed";
        default:                 // Unknown
            return "unknown";
    }
}
