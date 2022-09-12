/*
*  record.h
*  --------
*  Header file for a network service table record
*/
#pragma once

#ifndef RECORD_H
#define RECORD_H

#include <map>
#include "../contracts/i_string_castable.h"
#include "../inet/sockets/svc_info.h"
#include "../utils/algorithm.h"
#include "svc_field.h"

namespace scan
{
    /**
    * @brief  Network application service table record.
    */
    class Record : public IStringCastable
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using algo       = Algorithm;
        using field      = SvcField;
        using field_map  = std::map<field, size_t>;
        using str_array  = std::array<std::string, 4>;
        using str_vector = std::vector<std::string>;
        using string     = std::string;

    public:  /* Fields */
        static bool hide_sum;  // Hide summary field

        string port;           // Port number
        string proto;          // Transport protocol
        string service;        // Service name
        string state;          // Port state
        string summary;        // Service summary

    public:  /* Constructors & Destructor */
        Record() = default;
        Record(const Record &t_rec);
        Record(Record &&) = default;

        Record(const string &t_port,
               const string &t_state,
               const string &t_service,
               const string &t_summary,
               const string &t_proto = "tcp") noexcept;

        explicit Record(const SvcInfo &t_info);

        virtual ~Record() = default;

    public:  /* Operators */
        Record &operator=(const Record &t_rec) noexcept;
        Record &operator=(Record &&) = default;
        Record &operator=(const str_array &t_fields) noexcept;

        operator str_array() const;
        operator str_vector() const;
        operator string() const override;

        string &operator[](const field &t_field);
        const string &operator[](const field &t_field) const;

        bool operator==(const Record &t_rec) const noexcept;
        bool operator!=(const Record &t_rec) const noexcept;

        friend std::ostream &operator<<(std::ostream &t_os, const Record &t_rec);

    public:  /* Methods */
        uint port_num() const;

        str_array pad_fields(const field_map &t_map) const;

    private:  /* Methods */
        string state_str(const HostState &t_state) const noexcept;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline std::ostream &operator<<(std::ostream &t_os, const Record &t_rec)
    {
        return t_os << static_cast<std::string>(t_rec);
    }
}

#endif // !RECORD_H
