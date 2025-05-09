/*
* @file
*     hostname.h
* @brief
*     Header file for a network hostname.
*/
#pragma once

#ifndef SCAN_HOSTNAME_H
#define SCAN_HOSTNAME_H

#include <string>
#include "../../utils/aliases.h"
#include "../net_aliases.h"

namespace scan
{
    /**
    * @brief
    *     Network hostname and address information.
    */
    class Hostname
    {
    private:  /* Fields */
        net_error_code_t m_ecode;  // Name lookup error code

        string m_addr;             // IPv4 address
        string m_name;             // Target hostname

    public:  /* Constructors & Destructor */
        Hostname() = default;
        Hostname(const Hostname&) = default;
        Hostname(Hostname&&) = default;
        Hostname(const string& t_name);

        virtual ~Hostname() = default;

    public:  /* Operators */
        Hostname& operator=(const Hostname&) = default;
        Hostname& operator=(Hostname&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const noexcept
        {
            return m_addr.empty() ? m_name : m_addr;
        }

        friend ostream& operator<<(ostream& t_os, const Hostname& t_hostname);

    public:  /* Methods */
        /**
        * @brief
        *     Reset the underlying hostname information.
        */
        constexpr void reset()
        {
            *this = Hostname{};
        }

        /**
        * @brief
        *     Determine whether the underlying hostname is
        *     valid and can be resolved as an IPv4 address.
        */
        constexpr bool valid() const noexcept
        {
            return !m_name.empty() && !m_addr.empty();
        }

        /**
        * @brief
        *     Get the most recent DNS name resolution socket error code.
        */
        constexpr net_error_code_t last_error() const noexcept
        {
            return m_ecode;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying resolved IPv4 address.
        */
        constexpr const string& addr() const noexcept
        {
            return m_addr;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying hostname. The underlying
        *     IPv4 address will be returned instead if name resolution failed.
        */
        constexpr const string& name() const noexcept
        {
            return m_name.empty() ? m_addr : m_name;
        }

        void name(const string& t_name);

        bool resolve(uint_t t_retries = 1U);
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream& operator<<(ostream& t_os, const Hostname& t_hostname)
    {
        return t_os << t_hostname.name();
    }
}

#endif // !SCAN_HOSTNAME_H
