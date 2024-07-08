/*
* @file
*     svc_info.h
* @brief
*     Header file for network application service information.
*/
#pragma once

#ifndef SCAN_SVC_INFO_H
#define SCAN_SVC_INFO_H

#include <string>
#include "../../containers/svc_field.h"
#include "../../contracts/i_string_castable.h"
#include "../../utils/algo.h"
#include "../../utils/aliases.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../net_aliases.h"
#include "../net_const_defs.h"
#include "endpoint.h"
#include "host_state.h"

namespace scan
{
    /**
    * @brief
    *     Network application service information.
    */
    class SvcInfo : public IStringCastable
    {
    private:  /* Type Aliases */
        using field_map = map<SvcField, size_t>;
        using field_t   = SvcField;
        using str_array = array<string, 4>;

    public:  /* Fields */
        static bool no_summary;  // Hide the summary field

        string addr;             // Target address or hostname
        string banner;           // Raw banner data
        string cipher;           // SSL/TLS cipher suite
        string issuer;           // SSL/TLS certificate issuer
        string proto;            // Transport protocol
        string service;          // Service name
        string subject;          // SSL/TLS certificate subject
        string summary;          // Service summary

        Request<> request;       // HTTP request message
        Response<> response;     // HTTP response message

    public:  /* Fields */
        HostState m_state;   // Target host state
        port_t m_port;       // Target port number

        string m_port_str;   // Target port number string
        string m_state_str;  // Target host state name

    public:  /* Constructors & Destructor */
        SvcInfo() noexcept;
        SvcInfo(const SvcInfo &) = default;
        SvcInfo(SvcInfo &&) = default;
        SvcInfo(const Endpoint &t_ep, const HostState &t_state = HostState::unknown);

        SvcInfo(const Endpoint &t_ep,
                const string &t_banner,
                const HostState &t_state = HostState::open);

        SvcInfo(const string &t_port_str,
                const string &t_state_str,
                const string &t_service,
                const string &t_summary,
                const bool &t_header = false);

        virtual ~SvcInfo() = default;

    public:  /* Operators */
        SvcInfo &operator=(const SvcInfo &) = default;
        SvcInfo &operator=(SvcInfo &&) = default;
        SvcInfo &operator=(const str_array &t_fields) noexcept;

        operator str_array() const noexcept;
        operator scan::string_vector() const noexcept;
        operator std::string() const override;

        const string &operator[](const field_t &t_field) const;
        string &operator[](const field_t &t_field);

        friend ostream &operator<<(ostream &t_os, const SvcInfo &t_info);

    public:  /* Methods */
        void parse(const string &t_banner);
        void reset() noexcept;
        void reset(const string &t_addr) noexcept;

        bool valid_state_str(const string &t_state_str) const noexcept;

        /**
        * @brief
        *     Get a constant reference to the underlying target host state.
        */
        constexpr const HostState &state() const noexcept
        {
            return m_state;
        }

        /**
        * @brief
        *     Get a reference to the underlying target host state.
        */
        constexpr HostState &state() noexcept
        {
            return m_state;
        }

        /**
        * @brief
        *     Set the value of the underlying target host state.
        */
        constexpr HostState &state(const HostState &t_state) noexcept
        {
            switch (m_state = t_state)
            {
                case HostState::open:
                    m_state_str = STATE_OPEN;
                    break;
                case HostState::closed:
                    m_state_str = STATE_CLOSED;
                    break;
                default:
                    m_state_str = STATE_UNKNOWN;
                    break;
            }
            return m_state;
        }

        /**
        * @brief
        *     Get the value of the underlying port number.
        */
        constexpr port_t port() const noexcept
        {
            return m_port;
        }

        /**
        * @brief
        *     Set the value of the underlying port number.
        */
        constexpr port_t set_port(const port_t &t_port)
        {
            if (t_port != PORT_NULL)
            {
                m_port = t_port;
                m_port_str = algo::fstr("%/%", m_port, proto);
            }
            return m_port;
        }

        string details(const bool &t_colorize = false) const;

        /**
        * @brief
        *     Get a constant reference to the underlying port number string.
        */
        constexpr const string &port_str() const noexcept
        {
            return m_port_str;
        }

        string &port_str(const string &t_port_str);
        const string &state_str() const noexcept;
        string &state_str(const string &t_state_str);

        str_array pad_fields(const field_map &t_map) const;

    private:  /* Methods */
        string abbreviate(const string &t_data, const size_t &t_len = 35) const;
        string req_details(const bool &t_colorize) const;
        string resp_details(const bool &t_colorize) const;
        string tls_details(const bool &t_colorize) const;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const SvcInfo &t_info)
    {
        return t_os << static_cast<string>(t_info);
    }
}

#endif // !SCAN_SVC_INFO_H
