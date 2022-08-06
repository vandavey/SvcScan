/*
*  svc_table.h
*  -----------
*  Header file for a network application service table
*/
#pragma once

#ifndef SVC_TABLE_H
#define SVC_TABLE_H

#include "generic/list.h"
#include "record.h"

namespace scan
{
    /**
    * @brief  Network application service table.
    */
    class SvcTable
    {
    private:  /* Type Aliases */
        using this_t = SvcTable;

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
        SvcTable(const SvcTable &t_table);
        SvcTable(SvcTable &&) = default;
        SvcTable(const string &t_addr, const vector<SvcInfo> &t_vect);

        virtual ~SvcTable() = default;

    private:  /* Constructors */
        SvcTable();

    public:  /* Operators */
        SvcTable &operator=(const SvcTable &) = default;
        SvcTable &operator=(SvcTable &&) = default;

        friend std::ostream &operator<<(std::ostream &t_os, const SvcTable &t_table);

    public:  /* Methods */
        void add(const SvcInfo &t_info);
        void add(const vector<SvcInfo> &t_vect);

        string str() const;

    private:  /* Methods */
        size_t max_width(const vector<Record> &t_vect, const field &t_field) const;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline std::ostream &operator<<(std::ostream &t_os, const SvcTable &t_table)
    {
        return t_os << t_table.str();
    }
}

#endif // !SVC_TABLE_H
