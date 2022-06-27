/*
*  hostname.h
*  ----------
*  Header file for a network hostname
*/
#pragma once

#ifndef HOSTNAME_H
#define HOSTNAME_H

#include <sdkddkver.h>
#include <boost/asio/ip/tcp.hpp>
#include "../net_util.h"

namespace
{
    namespace error = boost::asio::error;
}

namespace scan
{
    /**
    * @brief  Network hostname and address information.
    */
    class Hostname
    {
    private:  /* Type Aliases */
        using uint = unsigned int;

        using error_code = boost::system::error_code;
        using io_context = boost::asio::io_context;
        using net        = NetUtil;
        using results_t  = boost::asio::ip::tcp::resolver::results_type;
        using string     = std::string;

    private:  /* Fields */
        error_code m_ecode;  // Lookup error code

        string m_addr;       // IPv4 address
        string m_name;       // Target hostname

    public:  /* Constructors & Destructor */
        Hostname() = default;
        Hostname(const Hostname &t_hostname);
        Hostname(const string &t_name);

        virtual ~Hostname() = default;

    public:  /* Operators */
        Hostname &operator=(const string &t_name);

        operator string() const noexcept;

        friend std::ostream &operator<<(std::ostream &t_os,
                                        const Hostname &t_hostname);

    public:  /* Methods */
        void reset();

        bool is_valid() const noexcept;
        bool resolve(const uint &t_retries = 1U);

        error_code last_error() const noexcept;

        const string &addr() const noexcept;
        const string &name() const noexcept;
        const string &name(const string &t_name);
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline std::ostream &operator<<(std::ostream &t_os, const Hostname &t_hostname)
    {
        return (t_os << t_hostname.name());
    }
}

#endif // !HOSTNAME_H
