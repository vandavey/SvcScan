/*
*  svc_info.h
*  ----------
*  Header file for network application service information
*/
#pragma once

#ifndef SVC_INFO_H
#define SVC_INFO_H

#include "../../containers/svc_field.h"
#include "../../contracts/i_string_castable.h"
#include "../../inet/net_defs.h"
#include "../../utils/algorithm.h"
#include "../http/request.h"
#include "../http/response.h"
#include "endpoint.h"
#include "host_state.h"

namespace scan
{
    /**
    * @brief  Network application service information.
    */
    class SvcInfo : public IStringCastable
    {
    private:  /* Type Aliases */
        using this_t = SvcInfo;

        using algo      = Algorithm;
        using field_map = map<SvcField, size_t>;
        using field_t   = SvcField;
        using stdu      = StdUtil;
        using str_array = array<string, 4>;

    public:  /* Fields */
        static bool no_summary;  // Hide the summary field

        string addr;             // Target address or hostname
        string banner;           // Raw banner data
        string proto;            // Transport protocol
        string service;          // Service name
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
        SvcInfo(const SvcInfo &t_info) noexcept;
        SvcInfo(SvcInfo &&) = default;
        SvcInfo(const Endpoint &ep, const HostState &t_state = HostState::unknown);

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
        SvcInfo &operator=(const SvcInfo &t_info) noexcept;
        SvcInfo &operator=(SvcInfo &&) = default;
        SvcInfo &operator=(const str_array &t_fields);

        operator str_array() const;
        operator scan::string_vector() const;
        operator std::string() const override;

        const string &operator[](const field_t &t_field) const;
        string &operator[](const field_t &t_field);

        friend ostream &operator<<(ostream &t_os, const SvcInfo &t_info);

    public:  /* Methods */
        void parse(const string &t_banner);
        void reset();
        void reset(const string &t_addr);

        bool valid_state_str(const string &t_state_str) const noexcept;

        const HostState &state() const noexcept;
        HostState &state() noexcept;
        HostState &state(const HostState &t_state) noexcept;

        port_t port() const noexcept;
        port_t set_port(const port_t &t_port);

        string details(const bool &t_colorize = false) const;
        const string &port_str() const noexcept;
        string &port_str(const string &t_port_str);
        string req_details(const bool &t_colorize = false) const;
        string resp_details(const bool &t_colorize = false) const;
        const string &state_str() const noexcept;
        string &state_str(const string &t_state_str);

        str_array pad_fields(const field_map &t_map) const;

    private:  /* Methods */
        string abbreviate(const string &t_data, const size_t &t_len = 35) const;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const SvcInfo &t_info)
    {
        return t_os << static_cast<string>(t_info);
    }
}

#endif // !SVC_INFO_H
