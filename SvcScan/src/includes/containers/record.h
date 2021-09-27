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
#include "../inet/hoststate.h"
#include "../inet/svcinfo.h"
#include "../properties/autoprop.h"
#include "svcfield.h"
#include "list.h"

namespace scan
{
    /// ***
    /// Service information table record
    /// ***
    class Record
    {
    private:  /* Types */
        using field  = SvcField;
        using string = std::string;

        using array_s  = std::array<string, 4>;
        using list_s   = List<string>;
        using vector_s = std::vector<string>;

        template<class T>
        using field_map = std::map<field, T>;

    public:  /* Fields */
        static AutoProp<bool> hide_info;  // Hide info field

        AutoProp<string> port;            // Port number
        AutoProp<string> state;           // Port state
        AutoProp<string> service;         // Service name
        AutoProp<string> info;            // Service info

    public:  /* Constructors & Destructor */
        Record() = default;
        Record(const Record &t_rec);

        Record(const string &t_port,
               const string &t_state,
               const string &t_service,
               const string &t_info);

        explicit Record(const SvcInfo &t_si);

        virtual ~Record() = default;

    public:  /* Operators */
        operator array_s() const;
        operator string() const;
        operator vector_s() const;

        Record &operator=(const array_s &t_fields);

        bool operator==(const Record &t_rec) const;
        bool operator!=(const Record &t_rec) const;

    public:  /* Methods */
        static bool is_less(const Record &t_lhs, const Record &t_rhs);

        void set_field(const field &t_sf, const string &t_value);

        string get_field(const field &t_sf) const;
        Record pad_fields(const field_map<size_t> &t_dict) const;

    private:  /* Methods */
        string state_str(const HostState &t_hs) const noexcept;
    };
}

#endif // !RECORD_H
