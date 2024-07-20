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
#include "../../utils/algo.h"
#include "../../utils/aliases.h"
#include "../../utils/literals.h"
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
    class SvcInfo
    {
    private:  /* Type Aliases */
        using field_map = map<SvcField, size_t>;
        using field_t   = SvcField;
        using str_array = array<string, 4>;

    public:  /* Fields */
        string addr;          // Target address or hostname
        string banner;        // Raw banner data
        string cipher;        // SSL/TLS cipher suite
        string issuer;        // SSL/TLS certificate issuer
        string proto;         // Transport protocol
        string service;       // Service name
        string subject;       // SSL/TLS certificate subject
        string summary;       // Service summary

        Request<> request;    // HTTP request message
        Response<> response;  // HTTP response message

    public:  /* Fields */
        HostState m_state;  // Target host state
        port_t m_port;      // Target port number

    public:  /* Constructors & Destructor */
        SvcInfo() noexcept;
        SvcInfo(const SvcInfo&) = default;
        SvcInfo(SvcInfo&&) = default;
        SvcInfo(const Endpoint& t_ep, const HostState& t_state = HostState::unknown);

        SvcInfo(const Endpoint& t_ep,
                const string& t_banner,
                const HostState& t_state = HostState::open);

        virtual ~SvcInfo() = default;

    public:  /* Operators */
        SvcInfo& operator=(const SvcInfo&) = default;
        SvcInfo& operator=(SvcInfo&&) = default;

    public:  /* Methods */
        /**
        * @brief
        *     Determine whether the given string can be parsed as a target host state.
        */
        constexpr bool valid_state_str(const string& t_state_str) const noexcept
        {
            return algo::any_equal(t_state_str, STATE_CLOSED, STATE_OPEN, STATE_UNKNOWN);
        }

        /**
        * @brief
        *     Get a constant reference to the underlying target host state.
        */
        constexpr const HostState& state() const noexcept
        {
            return m_state;
        }

        /**
        * @brief
        *     Get a reference to the underlying target host state.
        */
        constexpr HostState& state() noexcept
        {
            return m_state;
        }

        /**
        * @brief
        *     Set the value of the underlying target host state.
        */
        constexpr HostState& state(const HostState& t_state) noexcept
        {
            return m_state = t_state;
        }

        /**
        * @brief
        *     Set the value of the underlying target host state.
        */
        constexpr HostState& state(const string& t_state_str) noexcept
        {
            if (t_state_str == STATE_OPEN)
            {
                state(HostState::open);
            }
            else if (t_state_str == STATE_CLOSED)
            {
                state(HostState::closed);
            }
            else  // Unknown or invalid state
            {
                state(HostState::unknown);
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
        constexpr port_t port(const port_t& t_port)
        {
            if (t_port != PORT_NULL)
            {
                m_port = t_port;
            }
            return m_port;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying port number string.
        */
        constexpr const string port_str() const
        {
            return algo::fstr("%/%", m_port, proto);
        }

        /**
        * @brief
        *     Get the string representation of the underlying host state.
        */
        constexpr string state_str() const noexcept
        {
            string state;

            switch (m_state)
            {
                case HostState::open:
                    state = STATE_OPEN;
                    break;
                case HostState::closed:
                    state = STATE_CLOSED;
                    break;
                default:
                    state = STATE_UNKNOWN;
                    break;
            }
            return state;
        }

        void parse(const string& t_banner);
        void reset() noexcept;
        void reset(const string& t_addr) noexcept;

        string details(const bool& t_colorize = false) const;

    private:  /* Methods */
        /**
        * @brief
        *     Abbreviate the given string based on the specified string length.
        */
        template<size_t N>
            requires(N > 0)
        constexpr string abbreviate(const string& t_data) const
        {
            const string abbrev_data{t_data.substr(0_st, N)};
            return t_data.size() > N ? algo::fstr("%...", abbrev_data) : abbrev_data;
        }

        string req_details(const bool& t_colorize) const;
        string resp_details(const bool& t_colorize) const;
        string tls_details(const bool& t_colorize) const;
    };
}

#endif // !SCAN_SVC_INFO_H
