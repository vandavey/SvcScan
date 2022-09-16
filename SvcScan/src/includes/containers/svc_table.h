/*
*  svc_table.h
*  -----------
*  Header file for a network application service table
*/
#pragma once

#ifndef SVC_TABLE_H
#define SVC_TABLE_H

#include "../utils/algorithm.h"
#include "generic/list.h"
#include "record.h"

namespace scan
{
    /**
    * @brief  Network application service table.
    */
    class SvcTable
    {
    public:  /* Type Aliases */
        using value_type     = Record;
        using const_iterator = Iterator<value_type>;
        using iterator       = const_iterator;

    private:  /* Type Aliases */
        using this_t = SvcTable;

        using algo      = Algorithm;
        using field     = SvcField;
        using field_map = std::map<field, size_t>;
        using stdu      = StdUtil;
        using string    = std::string;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Fields */
        string m_addr;            // Scan target
        List<value_type> m_list;  // Record list

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
        void sort();

        const value_type *data() const noexcept;
        value_type *data() noexcept;

        iterator begin() const noexcept;
        iterator end() const noexcept;

        const string &addr() const noexcept;
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
