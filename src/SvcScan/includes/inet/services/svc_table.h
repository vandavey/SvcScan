/*
* @file
*     svc_table.h
* @brief
*     Header file for a network application service table.
*/
#pragma once

#ifndef SCAN_SVC_TABLE_H
#define SCAN_SVC_TABLE_H

#include <string>
#include "../../concepts/concepts.h"
#include "../../console/args.h"
#include "../../ranges/algo.h"
#include "../../ranges/list.h"
#include "../../utils/aliases.h"
#include "../../utils/literals.h"
#include "../net_aliases.h"
#include "svc_field.h"
#include "svc_info.h"

namespace scan
{
    /**
    * @brief
    *     Network application service table.
    */
    class SvcTable
    {
    private:  /* Type Aliases */
        using size_map = map<SvcField, size_t>;

    private:  /* Fields */
        shared_ptr<Args> m_argsp;  // Command-line arguments smart pointer

        string m_addr;             // Scan target hostname or address
        List<SvcInfo> m_list;      // Service information list

    public:  /* Constructors & Destructor */
        SvcTable() = default;
        SvcTable(const SvcTable&) = delete;
        SvcTable(SvcTable&&) = default;

        template<RangeOf<scan::SvcInfo> R>
        SvcTable(const string& t_addr, shared_ptr<Args> t_argsp, R&& t_range);

        virtual ~SvcTable() = default;

    public:  /* Operators */
        SvcTable& operator=(const SvcTable&) = delete;
        SvcTable& operator=(SvcTable&&) = default;

        friend ostream& operator<<(ostream& t_os, const SvcTable& t_table);

    public:  /* Methods */
        /**
        * @brief
        *     Append the given range of values to the underlying list.
        */
        template<RangeOf<SvcInfo> R>
        constexpr void push_back(R&& t_range)
        {
            m_list.push_back(std::forward<R>(t_range));
        }

        /**
        * @brief
        *     Sort the underlying service list by port number.
        */
        constexpr void sort()
        {
            MemberFuncPtr auto proj_func_ptr =
                static_cast<port_t (SvcInfo::*)() const>(&SvcInfo::port);

            algo::sort(m_list, {}, proj_func_ptr);
        }

        /**
        * @brief
        *     Determine whether the underlying service information list is empty.
        */
        constexpr bool empty() const noexcept
        {
            return size() == 0;
        }

        /**
        * @brief
        *     Get the size of the underlying service information list.
        */
        constexpr size_t size() const noexcept
        {
            return m_list.size();
        }

        /**
        * @brief
        *     Get a constant reference to the underlying target hostname or IPv4 address.
        */
        constexpr const string& addr() const noexcept
        {
            return m_addr;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying service information list.
        */
        constexpr const List<SvcInfo>& values() const noexcept
        {
            return m_list;
        }

        string str(bool t_colorize = false) const;
        string table_str(bool t_colorize = false) const;

        const Args& args() const;

        size_map make_size_map() const;

    private:  /* Methods */
        /**
        * @brief
        *     Get the maximum size for the service field
        *     corresponding to the given field enumeration type.
        */
        constexpr size_t max_field_size(SvcField t_field) const
        {
            size_t max_size{4_sz};

            for (size_t field_size{0_sz}; const SvcInfo& svc_info : m_list)
            {
                switch (t_field)
                {
                    case SvcField::service:
                        field_size = (algo::max)(svc_info.service.size(), 7_sz);
                        break;
                    case SvcField::state:
                        field_size = (algo::max)(svc_info.state_str().size(), 5_sz);
                        break;
                    case SvcField::port:
                        field_size = (algo::max)(svc_info.port_str().size(), 4_sz);
                        break;
                    case SvcField::summary:
                        field_size = (algo::max)(svc_info.summary.size(), 4_sz);
                        break;
                    default:
                        break;
                }
                max_size = (algo::max)(max_size, field_size);
            }
            return max_size;
        }

        string details_str(bool t_colorize = false) const;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream& operator<<(ostream& t_os, const SvcTable& t_table)
    {
        return t_os << t_table.str();
    }
}

/**
* @brief
*     Initialize the object.
*/
template<scan::RangeOf<scan::SvcInfo> R>
inline scan::SvcTable::SvcTable(const string& t_addr,
                                shared_ptr<Args> t_argsp,
                                R&& t_range)
{
    m_addr = t_addr;
    m_argsp = t_argsp;

    push_back(std::forward<R>(t_range));
    sort();
}

#endif // !SCAN_SVC_TABLE_H
