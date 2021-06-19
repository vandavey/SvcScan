/*
*  record.cpp
*  ----------
*  Source file for a table record
*/
#include "includes/container/record.h"

scan::AutoProp<bool> scan::Record::hide_info{ false };

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const Record &t_rec)
{
    port = t_rec.port;
    state = t_rec.state;
    service = t_rec.service;
    info = t_rec.info;
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
    operator=(list_s::copy_n<4>(t_il));
}

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const SvcInfo &t_si)
{
    port = t_si.port + "/tcp";
    state = state_str(t_si.state);
    service = t_si.service;
    info = t_si.info;
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator array_s() const
{
    return array_s{ port, state, service, info };
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator string() const
{
    const string delim{ hide_info ? "    " : "   " };
    return list_s::join(operator vector_s(), delim);
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator vector_s() const
{
    return vector_s{ port, state, service, info };
}

/// ***
/// Assignment operator overload
/// ***
scan::Record &scan::Record::operator=(const array_s &t_fields)
{
    port = t_fields[0];
    state = t_fields[1];
    service = t_fields[2];
    info = t_fields[3];

    return *this;
}

/// ***
/// Equality operator overload
/// ***
bool scan::Record::operator==(const Record &t_rec) const
{
    return operator array_s() == t_rec.operator array_s();
}

/// ***
/// Inequality operator overload
/// ***
bool scan::Record::operator!=(const Record &t_rec) const
{
    return operator array_s() != t_rec.operator array_s();
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
        case field::info:     // Service information
            return info;
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
        case field::info:     // Service information
            info = t_value;
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
        const field field{ pair.first };
        const size_t width{ pair.second };

        const size_t field_width{ get_field(pair.first).size() };

        // Invalid maximum width
        if (width < field_width)
        {
            throw ArgEx("t_dict", "Invalid key value (size_t)");
        }
        const size_t delta{ width - field_width };

        // Append padding to field
        if (delta > 0)
        {
            const string value{ get_field(field) + string(delta, ' ') };
            clone.set_field(field, value);
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
