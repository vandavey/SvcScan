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
#include "../net/svcinfo.h"
#include "svcfield.h"
#include "record.h"

namespace Scan
{
    class SvcTable
    {
    private:  /* Types */
        using string = std::string;

        using list_r = List<Record>;
        using vector_s = std::vector<string>;
        using vector_r = std::vector<Record>;
        using vector_si = std::vector<SvcInfo>;

        template<class T>
        using map_sf = std::map<SvcField, T>;

    private:  /* Fields */
        string m_addr;  // Scan target
        list_r m_list;  // Record list

    public:  /* Constructors & Destructor */
        SvcTable(const SvcTable &st);
        SvcTable(const string &addr, const vector_si &vect);

        virtual ~SvcTable() = default;

    protected:  /* Constructors */
        SvcTable();

    public:  /* Operators */
        const Record operator[](const size_t &index) const;
        friend std::ostream &operator<<(std::ostream &os, const SvcTable &st);

    public:  /* Methods */
        void add(const SvcInfo &si);
        void add(const vector_si &vect);

        const string str() const;

    private:  /* Methods */
        const int max_width(const SvcField &sf) const;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const SvcTable &st)
    {
        return (os << st.str());
    }
}

#endif // !SVC_TABLE_H
