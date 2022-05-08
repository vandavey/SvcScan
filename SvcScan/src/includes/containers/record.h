/*
*  record.h
*  --------
*  Header file for a table record
*/
#pragma once

#ifndef RECORD_H
#define RECORD_H

#include <map>
#include "../contracts/istringcastable.h"
#include "../inet/sockets/svcinfo.h"
#include "svcfield.h"

namespace scan
{
    /// ***
    /// Service information table record
    /// ***
    class Record : public IStringCastable
    {
    private:  /* Type Aliases */
        using array_s  = std::array<std::string, 4>;
        using field    = SvcField;
        using string   = std::string;
        using vector_s = std::vector<string>;

        template<class T>
        using field_map = std::map<field, T>;

    public:  /* Fields */
        static bool hide_sum;  // Hide summary field

        string port;           // Port number
        string state;          // Port state
        string service;        // Service name
        string summary;        // Service summary

    public:  /* Constructors & Destructor */
        Record() = default;
        Record(const Record &t_rec);

        Record(const string &t_port,
               const string &t_state,
               const string &t_service,
               const string &t_summary) noexcept;

        explicit Record(const SvcInfo &t_si);

        virtual ~Record() = default;

    public:  /* Operators */
        Record &operator=(const Record &t_rec) noexcept;
        Record &operator=(const array_s &t_fields) noexcept;

        operator array_s() const;
        operator string() const override;
        operator vector_s() const;

        string &operator[](const field &t_sf);
        const string &operator[](const field &t_sf) const;

        bool operator==(const Record &t_rec) const noexcept;
        bool operator!=(const Record &t_rec) const noexcept;

        friend std::ostream &operator<<(std::ostream &t_os, const Record &t_rec);

    public:  /* Methods */
        static bool is_less_predicate(const Record &t_lhs, const Record &t_rhs);

        Record pad_fields(const field_map<size_t> &t_dict) const;

    private:  /* Methods */
        string state_str(const HostState &t_hs) const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const Record &t_rec)
    {
        return (t_os << static_cast<std::string>(t_rec));
    }
}

#endif // !RECORD_H
