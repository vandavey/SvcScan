/*
* @file
*     hostname.h
* @brief
*     Header file for a network hostname.
*/
#pragma once

#ifndef HOSTNAME_H
#define HOSTNAME_H

#include "../../utils/type_defs.h"
#include "../net_defs.h"
#include "../net_util.h"

namespace scan
{
    /**
    * @brief
    *     Network hostname and address information.
    */
    class Hostname
    {
    private:  /* Type Aliases */
        using algo = Algorithm;
        using net  = NetUtil;

    private:  /* Fields */
        error_code m_ecode;  // Lookup error code

        string m_addr;       // IPv4 address
        string m_name;       // Target hostname

    public:  /* Constructors & Destructor */
        Hostname() = default;
        Hostname(const Hostname &t_hostname);
        Hostname(Hostname &&) = default;
        Hostname(const string &t_name);

        virtual ~Hostname() = default;

    public:  /* Operators */
        Hostname &operator=(const Hostname &) = default;
        Hostname &operator=(Hostname &&) = default;
        Hostname &operator=(const string &t_name);

        operator std::string() const noexcept;

        friend ostream &operator<<(ostream &t_os, const Hostname &t_hostname);

    public:  /* Methods */
        void reset();

        bool is_valid() const noexcept;
        bool resolve(const uint_t &t_retries = 1U);

        error_code last_error() const noexcept;

        const string &addr() const noexcept;
        const string &name() const noexcept;
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

#endif // !HOSTNAME_H
