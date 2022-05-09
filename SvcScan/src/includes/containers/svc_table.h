/*
*  svc_table.h
*  -----------
*  Header file for a two dimensional service table
*/
#pragma once

#ifndef SVC_TABLE_H
#define SVC_TABLE_H

#include "../utils/arg_parser.h"
#include "record.h"

namespace scan
{
    /// ***
    /// Two dimensional network service container
    /// ***
    class SvcTable
    {
    private:  /* Type Aliases */
        using field   = SvcField;
        using sstream = std::stringstream;
        using stdu    = StdUtil;
        using string  = std::string;

        template<class T>
        using field_map = std::map<field, T>;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Fields */
        string m_addr;        // Scan target
        List<Record> m_list;  // Record list

    public:  /* Constructors & Destructor */
        SvcTable(const SvcTable &t_st);
        SvcTable(const string &t_addr, const vector<SvcInfo> &t_vect);

        virtual ~SvcTable() = default;

    private:  /* Constructors */
        SvcTable();

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &t_os, const SvcTable &t_st);

    public:  /* Methods */
        void add(const SvcInfo &t_si);
        void add(const vector<SvcInfo> &t_vect);

        string str() const;

    private:  /* Methods */
        size_t max_width(const vector<Record> &t_vect, const field &t_sf) const;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const SvcTable &t_st)
    {
        return (t_os << t_st.str());
    }
}

#endif // !SVC_TABLE_H
