/*
*  svctable.h
*  ----------
*  Header file for a two dimensional container
*/
#pragma once

#ifndef SVC_TABLE_H
#define SVC_TABLE_H

#include <map>
#include <string>
#include <vector>
#include "../inet/svcinfo.h"
#include "record.h"
#include "svcfield.h"

namespace scan
{
    /// ***
    /// Two dimensional network service container
    /// ***
    class SvcTable
    {
    private:  /* Types */
        using field  = SvcField;
        using string = std::string;

        using list_r    = List<Record>;
        using vector_r  = std::vector<Record>;
        using vector_si = std::vector<SvcInfo>;

        template<class T>
        using map_sf = std::map<field, T>;

    private:  /* Fields */
        string m_addr;  // Scan target
        list_r m_list;  // Record list

    public:  /* Constructors & Destructor */
        SvcTable(const SvcTable &t_st);
        SvcTable(const string &t_addr, const vector_si &t_vect);

        virtual ~SvcTable() = default;

    private:  /* Constructors */
        SvcTable();

    public:  /* Operators */
        Record operator[](const size_t &t_idx) const;

        friend std::ostream &operator<<(std::ostream &t_os,
                                        const SvcTable &t_st);

    public:  /* Methods */
        void add(const SvcInfo &t_si);
        void add(const vector_si &t_vect);

        string str() const;

    private:  /* Methods */
        int get_width(const vector_r &t_vect, const field &t_sf) const;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const SvcTable &t_st)
    {
        if (!Parser::verbose)
        {
            t_os << Util::LF;
        }
        return (t_os << t_st.str());
    }
}

#endif // !SVC_TABLE_H
