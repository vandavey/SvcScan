/*
*  record.h
*  --------
*  Header file for a table record
*/
#pragma once

#ifndef RECORD_H
#define RECORD_H

#include <array>
#include <map>
#include "../net/hoststate.h"
#include "../net/svcinfo.h"
#include "../properties/property.h"
#include "svcfield.h"
#include "list.h"

namespace Scan
{
    /// ***
    /// Service information table record (row)
    /// ***
    class Record
    {
    private:  /* Types */
        using string = std::string;

        using array_s = std::array<string, 4>;
        using list_s = List<string>;
        using vector_s = std::vector<string>;

        template<class T>
        using map_sf = std::map<SvcField, T>;

    public:  /* Fields */
        Property<string> port;     // Port number
        Property<string> state;    // Port state
        Property<string> service;  // Service name
        Property<string> version;  // Service version

    private:  /* Fields */
        string m_port;           // 'port' backing field
        string m_state;          // 'state' backing field
        string m_service;        // 'service' backing field
        string m_version;        // 'version' backing field

        map_sf<string &> m_map;  // Field lookup map

    public:  /* Constructors & Destructor */
        Record();
        Record(const Record &row);
        explicit Record(const array_s &fields);
        explicit Record(const SvcInfo &si);

        virtual ~Record() = default;

    public:  /* Operators */
        operator const array_s() const;
        operator const std::string() const;
        operator const vector_s() const;

        const bool operator==(const Record &row) const;
        const bool operator!=(const Record &row) const;

        const string &operator[](const SvcField &sf) const;
        string &operator[](const SvcField &sf);

    public:  /* Methods */
        const Record pad_fields(const map_sf<size_t> &dict) const;

    private:  /* Methods */
        const string state_str(const HostState &hs) const noexcept;

        Record &swap(const Record &row) noexcept;
    };
}

#endif // !RECORD_H
