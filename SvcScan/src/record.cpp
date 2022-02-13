/*
*  record.cpp
*  ----------
*  Source file for a table record
*/
#include "includes/containers/record.h"

bool scan::Record::hide_sum{ false };

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const Record &t_rec)
{
    operator=(t_rec);
}

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const string &t_port,
                     const string &t_state,
                     const string &t_service,
                     const string &t_summary) noexcept :
    port(t_port),
    state(t_state),
    service(t_service),
    summary(t_summary) {
}

/// ***
/// Initialize the object
/// ***
scan::Record::Record(const SvcInfo &t_si)
{
    port = Util::fstr("%/tcp", t_si.port);
    state = state_str(t_si.state);
    service = t_si.service;
    summary = t_si.summary;
}

/// ***
/// Assignment operator overload
/// ***
scan::Record &scan::Record::operator=(const Record &t_rec) noexcept
{
    port = t_rec.port;
    state = t_rec.state;
    service = t_rec.service;
    summary = t_rec.summary;

    return *this;
}

/// ***
/// Assignment operator overload
/// ***
scan::Record &scan::Record::operator=(const array_s &t_fields) noexcept
{
    port = t_fields[0];
    state = t_fields[1];
    service = t_fields[2];
    summary = t_fields[3];

    return *this;
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator array_s() const
{
    return array_s{ port, state, service, summary };
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator string() const
{
    const string delim{ hide_sum ? "    " : "   " };
    return list_s::join(operator vector_s(), delim);
}

/// ***
/// Cast operator overload
/// ***
scan::Record::operator vector_s() const
{
    return vector_s{ port, state, service, summary };
}

/// ***
/// Subscript operator overload
/// ***
std::string &scan::Record::operator[](const field &t_sf)
{
    string *field_ptr;

    switch (t_sf)
    {
        case field::port:     // Port number
            field_ptr = &port;
            break;
        case field::service:  // Service name
            field_ptr = &service;
            break;
        case field::state:    // Target state
            field_ptr = &state;
            break;
        case field::info:     // Service information
            field_ptr = &summary;
            break;
        default:
            throw ArgEx{ "t_sf", "Invalid field enum" };
    }
    return *field_ptr;
}

/// ***
/// Subscript operator overload
/// ***
const std::string &scan::Record::operator[](const field &t_sf) const
{
    const string *field_ptr;

    switch (t_sf)
    {
        case field::port:     // Port number
            field_ptr = &port;
            break;
        case field::service:  // Service name
            field_ptr = &service;
            break;
        case field::state:    // Target state
            field_ptr = &state;
            break;
        case field::info:     // Service information
            field_ptr = &summary;
            break;
        default:
            throw ArgEx{ "t_sf", "Invalid field enum" };
    }
    return *field_ptr;
}

/// ***
/// Equality operator overload
/// ***
bool scan::Record::operator==(const Record &t_rec) const noexcept
{
    return t_rec.port == port
        && t_rec.state == state
        && t_rec.service == service
        && t_rec.summary == summary;
}

/// ***
/// Inequality operator overload
/// ***
bool scan::Record::operator!=(const Record &t_rec) const noexcept
{
    return !operator==(t_rec);
}

/// ***
/// Determine if the left-hand record's port number is less
/// ***
bool scan::Record::is_less_predicate(const Record &t_lhs, const Record &t_rhs)
{
    return std::stoi(t_lhs.port) < std::stoi(t_rhs.port);
}

/// ***
/// Add padding to all the fields in the vector and return as copy
/// ***
scan::Record scan::Record::pad_fields(const field_map<size_t> &t_dict) const
{
    Record clone{ *this };

    // Add padding to fields
    for (const field_map<size_t>::value_type &pair : t_dict)
    {
        // Avoid trailing whitespace
        if (pair.first == field::info)
        {
            continue;
        }
        const field rec_field{ pair.first };
        const size_t max_width{ pair.second };
        const size_t width{ operator[](rec_field).size() };

        // Invalid maximum width
        if (max_width < width)
        {
            throw ArgEx{ "t_dict", "Invalid key value (size_t)" };
        }
        const size_t delta{ max_width - width };

        // Append padding to field
        if (delta > 0)
        {
            clone[rec_field] = operator[](rec_field) + string(delta, ' ');
        }
    }
    return clone;
}

/// ***
/// Get the string equivalent of the given host state
/// ***
std::string scan::Record::state_str(const HostState &t_hs) const noexcept
{
    string state;

    switch (t_hs)
    {
        case HostState::open:    // Open
            state = "open";
            break;
        case HostState::closed:  // Closed
            state = "closed";
            break;
        default:                 // Unknown
            state = "unknown";
            break;
    }
    return state;
}
