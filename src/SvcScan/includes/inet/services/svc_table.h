/*
* @file
*     svc_table.h
* @brief
*     Header file for a network application service table.
*/
#pragma once

#ifndef SCAN_SVC_TABLE_H
#define SCAN_SVC_TABLE_H

#include "../../concepts/concepts.h"
#include "../../ranges/list.h"
#include "../../utils/algo.h"
#include "../../utils/aliases.h"
#include "../../utils/args.h"
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
        constexpr SvcTable() = default;
        constexpr SvcTable(const SvcTable&) = default;
        SvcTable(SvcTable&& t_table) noexcept;

        template<scan::Range R>
            requires scan::RangeValue<R, scan::SvcInfo>
        SvcTable(const string& t_addr, shared_ptr<Args> t_argsp, const R& t_range);

        virtual constexpr ~SvcTable() = default;

    public:  /* Operators */
        SvcTable& operator=(const SvcTable&) = default;
        SvcTable& operator=(SvcTable&& t_table) noexcept;

        friend ostream& operator<<(ostream& t_os, const SvcTable& t_table);

    public:  /* Methods */
        /**
        * @brief
        *     Add a new record to the underlying list of service information.
        */
        constexpr void add(const SvcInfo& t_info)
        {
            m_list.add(t_info);
        }

        /**
        * @brief
        *     Add new records to the underlying list of service information.
        */
        template<Range R>
            requires RangeValue<R, SvcInfo>
        constexpr void add(const R& t_range)
        {
            m_list.add(t_range);
        }

        /**
        * @brief
        *     Sort the underlying service list by port number.
        */
        constexpr void sort()
        {
            MemberFuncPtr auto proj_func_ptr =
                static_cast<port_t (SvcInfo::*)() const>(&SvcInfo::port);

            algo::sort(m_list.vector(), ranges::less{}, proj_func_ptr);
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
            size_t max_size{4_st};

            for (size_t field_size{0_st}; const SvcInfo& svc_info : m_list)
            {
                switch (t_field)
                {
                    case SvcField::service:
                        field_size = algo::maximum(svc_info.service.size(), 7_st);
                        break;
                    case SvcField::state:
                        field_size = algo::maximum(svc_info.state_str().size(), 5_st);
                        break;
                    case SvcField::port:
                        field_size = algo::maximum(svc_info.port_str().size(), 4_st);
                        break;
                    case SvcField::summary:
                        field_size = algo::maximum(svc_info.summary.size(), 4_st);
                        break;
                    default:
                        break;
                }
                max_size = algo::maximum(max_size, field_size);
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
template<scan::Range R>
    requires scan::RangeValue<R, scan::SvcInfo>
inline scan::SvcTable::SvcTable(const string& t_addr,
                                shared_ptr<Args> t_argsp,
                                const R& t_range)
{
    m_addr = t_addr;
    m_argsp = t_argsp;

    add(t_range);
    sort();
}

#endif // !SCAN_SVC_TABLE_H
