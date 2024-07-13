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

namespace scan
{
    /**
    * @brief
    *     Network hostname and address information.
    */
    class Hostname
    {
    private:  /* Fields */
        error_code m_ecode;  // Lookup error code

        string m_addr;       // IPv4 address
        string m_name;       // Target hostname

    public:  /* Constructors & Destructor */
        Hostname() = default;
        Hostname(const Hostname &) = default;
        Hostname(Hostname &&) = default;
        Hostname(const string &t_name);

        virtual ~Hostname() = default;

    public:  /* Operators */
        Hostname &operator=(const Hostname &) = default;
        Hostname &operator=(Hostname &&) = default;
        Hostname &operator=(const string &t_name);

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const noexcept
        {
            return m_addr.empty() ? m_name : m_addr;
        }

        friend ostream &operator<<(ostream &t_os, const Hostname &t_hostname);

    public:  /* Methods */
        /**
        * @brief
        *     Reset the underlying hostname information.
        */
        constexpr void reset()
        {
            *this = Hostname();
        }

        /**
        * @brief
        *     Determine whether the underlying hostname can be resolved as an IPv4 address.
        */
        constexpr bool is_valid() const noexcept
        {
            return !m_name.empty() && !m_addr.empty();
        }

        /**
        * @brief
        *     Get the most recent DNS name resolution socket error code.
        */
        constexpr error_code last_error() const noexcept
        {
            return m_ecode;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying resolved IPv4 address.
        */
        constexpr const string &addr() const noexcept
        {
            return m_addr;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying hostname. The underlying
        *     IPv4 address will be returned instead if name resolution failed.
        */
        constexpr const string &name() const noexcept
        {
            return m_name.empty() ? m_addr : m_name;
        }

        bool resolve(const uint_t &t_retries = 1U);

        const string &name(const string &t_name);
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const Hostname &t_hostname)
    {
        return t_os << t_hostname.name();
    }
}

#endif // !SCAN_HOSTNAME_H
