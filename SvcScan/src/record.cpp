/*
*  record.cpp
*  ----------
*  Source file for a network service table record
*/
#include "includes/containers/generic/list.h"
#include "includes/containers/record.h"
#include "includes/except/runtime_ex.h"

/**
* @brief  Hide the summary field when casting to a string.
*/
bool scan::Record::hide_sum{ false };

/**
* @brief  Initialize the object.
*/
scan::Record::Record()
{
    proto = net::PROTOCOL;
}

/**
* @brief  Initialize the object.
*/
scan::Record::Record(const Record &t_rec)
{
    *this = t_rec;
}

/**
* @brief  Initialize the object.
*/
scan::Record::Record(const string &t_port,
                     const string &t_state,
                     const string &t_service,
                     const string &t_summary,
                     const string &t_proto) noexcept {
    port = t_port;
    proto = t_proto;
    service = t_service;
    state = t_state;
    summary = t_summary;
}

/**
* @brief  Initialize the object.
*/
scan::Record::Record(const SvcInfo &t_info)
{
    port = algo::fstr("%/%", t_info.port, t_info.proto);
    proto = t_info.proto;
    service = t_info.service;
    state = state_str(t_info.state);
    summary = t_info.summary;
}

/**
* @brief  Copy assignment operator overload.
*/
scan::Record &scan::Record::operator=(const Record &t_rec) noexcept
{
    port = t_rec.port;
    proto = t_rec.proto;
    service = t_rec.service;
    state = t_rec.state;
    summary = t_rec.summary;

    return *this;
}

/**
* @brief  Assignment operator overload.
*/
scan::Record &scan::Record::operator=(const str_array &t_fields) noexcept
{
    port = t_fields[0];
    service = t_fields[1];
    state = t_fields[2];
    summary = t_fields[3];

    return *this;
}

/**
* @brief  Cast operator overload.
*/
scan::Record::operator str_array() const
{
    return str_array{ port, service, state, summary };
}

/**
* @brief  Cast operator overload.
*/
scan::Record::operator str_vector() const
{
    return str_vector{ port, service, state, summary };
}

/**
* @brief  Cast operator overload.
*/
scan::Record::operator string() const
{
    return algo::join(operator str_vector(), hide_sum ? "    " : "   ");
}

/**
* @brief  Subscript operator overload.
*/
std::string &scan::Record::operator[](const field &t_field)
{
    string *fieldp;

    switch (t_field)
    {
        case field::port:
            fieldp = &port;
            break;
        case field::proto:
            fieldp = &proto;
            break;
        case field::service:
            fieldp = &service;
            break;
        case field::state:
            fieldp = &state;
            break;
        case field::info:
            fieldp = &summary;
            break;
        default:
            fieldp = nullptr;
    }
    return *fieldp;
}

/**
* @brief  Subscript operator overload.
*/
const std::string &scan::Record::operator[](const field &t_field) const
{
    const string *fieldp;

    switch (t_field)
    {
        case field::port:
            fieldp = &port;
            break;
        case field::proto:
            fieldp = &proto;
            break;
        case field::service:
            fieldp = &service;
            break;
        case field::state:
            fieldp = &state;
            break;
        case field::info:
            fieldp = &summary;
            break;
        default:
            fieldp = nullptr;
    }
    return *fieldp;
}

/**
* @brief  Equality operator overload.
*/
bool scan::Record::operator==(const Record &t_rec) const noexcept
{
    const bool eq_ports{ port == t_rec.port };
    const bool eq_protos{ proto == t_rec.proto };
    const bool eq_services{ service == t_rec.service };
    const bool eq_states{ state == t_rec.state };
    const bool eq_summaries{ summary == t_rec.summary };

    return eq_ports && eq_protos && eq_services && eq_states && eq_summaries;
}

/**
* @brief  Inequality operator overload.
*/
bool scan::Record::operator!=(const Record &t_rec) const noexcept
{
    return !operator==(t_rec);
}

/**
* @brief  Get the underlying port number as an unsigned integer.
*/
unsigned int scan::Record::port_num() const
{
    const string num_str{ port.substr(0, port.find(algo::fstr("/%", proto))) };
    return static_cast<uint>(std::stoi(num_str));
}

/**
* @brief  Get a copy of the current record with its fields padded.
*/
scan::Record::str_array scan::Record::pad_fields(const field_map &t_map) const
{
    Record record{ *this };

    for (const field_map::value_type &pair : t_map)
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
            throw RuntimeEx{ "Record::pad_fields", "Invalid width value in map" };
        }
        const size_t delta{ max_width - width };

        if (delta > 0)
        {
            record[rec_field] = operator[](rec_field) + string(delta, ' ');
        }
    }
    return record;
}

/**
* @brief  Get the string equivalent of the given host state.
*/
std::string scan::Record::state_str(const HostState &t_state) const noexcept
{
    string state;

    switch (t_state)
    {
        case HostState::open:
            state = "open";
            break;
        case HostState::closed:
            state = "closed";
            break;
        default:
            state = "unknown";
            break;
    }
    return state;
}
