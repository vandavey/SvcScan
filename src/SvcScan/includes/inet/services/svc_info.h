/*
* @file
*     svc_info.h
* @brief
*     Header file for network application service information.
*/
#pragma once

#ifndef SCAN_SVC_INFO_H
#define SCAN_SVC_INFO_H

#include <array>
#include <string>
#include "../../ranges/algo.h"
#include "../../utils/aliases.h"
#include "../../utils/literals.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../net_aliases.h"
#include "../net_const_defs.h"
#include "../sockets/endpoint.h"
#include "../sockets/host_state.h"

namespace scan
{
    /**
    * @brief
    *     Network application service information.
    */
    class SvcInfo
    {
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

    private:  /* Fields */
        HostState m_state;  // Target host state
        port_t m_port;      // Target port number

    public:  /* Constructors & Destructor */
        SvcInfo() noexcept;
        SvcInfo(const SvcInfo&) = default;
        SvcInfo(SvcInfo&&) = default;
        SvcInfo(const Endpoint& t_ep, HostState t_state = HostState::unknown);

        SvcInfo(const Endpoint& t_ep,
                const string& t_banner,
                HostState t_state = HostState::open);

        virtual ~SvcInfo() = default;

    public:  /* Operators */
        SvcInfo& operator=(const SvcInfo&) = default;
        SvcInfo& operator=(SvcInfo&&) = default;

    public:  /* Methods */
        /**
        * @brief
        *     Parse service details from the given CSV record line.
        *     Underlying fields are only updated when they are empty.
        */
        constexpr void parse_csv_line(const string& t_csv_line)
        {
            if (!t_csv_line.empty())
            {
                const string csv_line{algo::erase(t_csv_line, "\"")};
                const string_array_t fields{algo::split<4>(csv_line, ",")};

                proto = proto.empty() ? fields[1] : proto;
                service = service.empty() ? fields[2] : service;
                summary = summary.empty() ? fields[3] : summary;
            }
        }

        /**
        * @brief
        *     Set the value of the underlying port number.
        */
        constexpr void port(port_t t_port) noexcept
        {
            if (t_port != PORT_NULL)
            {
                m_port = t_port;
            }
        }

        /**
        * @brief
        *     Set the value of the underlying target host state.
        */
        constexpr void state(HostState t_state) noexcept
        {
            m_state = t_state;
        }

        /**
        * @brief
        *     Set the value of the underlying target host state.
        */
        constexpr void state(const string& t_state_str) noexcept
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
        }

        /**
        * @brief
        *     Determine whether the given string can be parsed as a target host state.
        */
        constexpr bool valid_state_str(const string& t_state_str) const noexcept
        {
            return algo::any_eq(t_state_str, STATE_CLOSED, STATE_OPEN, STATE_UNKNOWN);
        }

        /**
        * @brief
        *     Get the value of the underlying target host state.
        */
        constexpr HostState state() const noexcept
        {
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

        void parse_banner(const string& t_banner);
        void reset() noexcept;
        void reset(const string& t_addr) noexcept;

        string details(bool t_colorize = false) const;

    private:  /* Methods */
        /**
        * @brief
        *     Abbreviate the given string based on the specified string length.
        */
        template<size_t N>
            requires(N > 0)
        constexpr string abbreviate(const string& t_data) const
        {
            const string abbrev_data{algo::up_to_first(t_data.substr(0_sz, N), " ")};
            return t_data.size() > N ? algo::fstr("%...", abbrev_data) : abbrev_data;
        }

        string request_details(bool t_colorize) const;
        string response_details(bool t_colorize) const;
        string tls_details(bool t_colorize) const;
    };
}

#endif // !SCAN_SVC_INFO_H
