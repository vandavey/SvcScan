/*
* @file
*     svc_table.h
* @brief
*     Header file for a network application service table.
*/
#pragma once

#ifndef SCAN_SVC_TABLE_H
#define SCAN_SVC_TABLE_H

#include "../inet/sockets/svc_info.h"
#include "../io/std_util.h"
#include "../utils/algorithm.h"
#include "../utils/args.h"
#include "../utils/type_defs.h"
#include "generic/iterator.h"
#include "generic/list.h"
#include "svc_field.h"

namespace scan
{
    /**
    * @brief
    *     Network application service table.
    */
    class SvcTable
    {
    public:  /* Type Aliases */
        using value_type     = SvcInfo;
        using const_iterator = Iterator<value_type>;
        using iterator       = const_iterator;

    private:  /* Type Aliases */
        using this_t = SvcTable;

        using algo      = Algorithm;
        using field_map = map<SvcField, size_t>;
        using field_t   = SvcField;
        using stdu      = StdUtil;

    private:  /* Fields */
        shared_ptr<Args> m_argsp;  // Command-line arguments smart pointer

        string m_addr;             // Scan target hostname or address
        List<value_type> m_list;   // Service information list

    public:  /* Constructors & Destructor */
        SvcTable();
        SvcTable(const SvcTable &) = default;
        SvcTable(SvcTable &&t_table) noexcept;

        SvcTable(const string &t_addr,
                 shared_ptr<Args> t_argsp,
                 const vector<value_type> &t_vect);

        virtual ~SvcTable() = default;

    public:  /* Operators */
        SvcTable &operator=(const SvcTable &) = default;
        SvcTable &operator=(SvcTable &&) = default;

        friend ostream &operator<<(ostream &t_os, const SvcTable &t_table);

    public:  /* Methods */
        void add(const value_type &t_info);
        void add(const vector<value_type> &t_vect);
        void sort();

        bool empty(const bool &t_inc_header = false) const noexcept;

        size_t size(const bool &t_inc_header = false) const noexcept;

        iterator begin() const noexcept;
        iterator end() const noexcept;

        const string &addr() const noexcept;
        string str(const bool &t_colorize = false) const;
        string table_str(const bool &t_colorize = false) const;

        const Args &args() const;

        field_map make_width_map() const;

        List<value_type> data() const;

    private:  /* Methods */
        size_t max_width(const field_t &t_field) const;

        string details_str(const bool &t_colorize = false) const;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const SvcTable &t_table)
    {
        return t_os << t_table.str();
    }
}

#endif // !SCAN_SVC_TABLE_H
