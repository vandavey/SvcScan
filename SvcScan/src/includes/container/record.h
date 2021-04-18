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
#include "../properties/autoprop.h"
#include "svcfield.h"
#include "list.h"

namespace scan
{
    /// ***
    /// Service information table record (row)
    /// ***
    class Record
    {
    private:  /* Types */
        using field = SvcField;
        using string = std::string;

        using array_s = std::array<string, 4>;
        using list_s = List<string>;
        using vector_s = std::vector<string>;

        template<class T>
        using map_sf = std::map<field, T>;

    public:  /* Fields */
        AutoProp<string> port;     // Port number
        AutoProp<string> state;    // Port state
        AutoProp<string> service;  // Service name
        AutoProp<string> version;  // Service version

    public:  /* Constructors & Destructor */
        Record() = default;
        Record(const Record &t_row);
        explicit Record(const array_s &t_fields);
        explicit Record(const SvcInfo &t_si);

        virtual ~Record() = default;

    public:  /* Operators */
        operator const array_s() const;
        operator const std::string() const;
        operator const vector_s() const;

        const bool operator==(const Record &t_row) const;
        const bool operator!=(const Record &t_row) const;

    public:  /* Methods */
        static const bool is_less(const Record &t_lhs, const Record &t_rhs);

        void set_field(const field &t_sf, const string &t_value);

        const string get_field(const field &t_sf) const;
        const Record pad_fields(const map_sf<size_t> &t_dict) const;

    private:  /* Methods */
        const string state_str(const HostState &t_hs) const noexcept;
    };
}

#endif // !RECORD_H
