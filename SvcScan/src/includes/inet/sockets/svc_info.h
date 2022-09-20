/*
*  svc_info.h
*  ----------
*  Header file for network application service information
*/
#pragma once

#ifndef SVC_INFO_H
#define SVC_INFO_H

#include <array>
#include <map>
#include <vector>
#include <sdkddkver.h>
#include <boost/beast/http/verb.hpp>
#include "../../containers/svc_field.h"
#include "../../contracts/i_string_castable.h"
#include "../../utils/algorithm.h"
#include "../http/http_msg.h"
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

        using uint = unsigned int;

        using algo       = Algorithm;
        using field_map  = std::map<SvcField, size_t>;
        using field_t    = SvcField;
        using header_map = HttpMsg::header_map;
        using header_t   = HttpMsg::header_t;
        using status_t   = boost::beast::http::status;
        using str_array  = std::array<std::string, 4>;
        using str_vector = std::vector<std::string>;
        using string     = std::string;
        using verb_t     = boost::beast::http::verb;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Fields */
        static bool no_summary;   // Hide the summary field

        verb_t req_method;        // HTTP request method
        status_t resp_status;     // HTTP response status

        HttpVersion req_httpv;    // HTTP request protocol version
        HttpVersion resp_httpv;   // HTTP response protocol version

        string addr;              // Target address or hostname
        string banner;            // Raw banner data
        string proto;             // Transport protocol
        string req_uri;           // HTTP request URI
        string service;           // Service name
        string summary;           // Service summary

        header_map req_headers;   // HTTP request headers
        header_map resp_headers;  // HTTP response headers

    public:  /* Fields */
        HostState m_state;   // Target host state
        uint m_port;         // Target port number

        string m_port_str;   // Target port number string
        string m_state_str;  // Target host state name

    public:  /* Constructors & Destructor */
        SvcInfo();
        SvcInfo(const SvcInfo &t_info);
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
        operator str_vector() const;
        operator string() const override;

        const string &operator[](const field_t &t_field) const;
        string &operator[](const field_t &t_field);

        bool operator==(const SvcInfo &t_info) const noexcept;
        bool operator!=(const SvcInfo &t_info) const noexcept;

        friend std::ostream &operator<<(std::ostream &t_os, const SvcInfo &t_info);

    public:  /* Methods */
        void parse(const string &t_banner);
        void reset();
        void reset(const string &t_addr);

        bool valid_state_str(const string &t_state_str) const noexcept;

        const HostState &state() const noexcept;
        HostState &state() noexcept;
        HostState &state(const HostState &t_state) noexcept;

        uint get_port() const noexcept;
        uint port() const noexcept;
        uint port(const uint &t_port);

        const string &port_str() const noexcept;
        string &port_str(const string &t_port_str);
        const string &state_str() const noexcept;
        string &state_str(const string &t_state_str);

        str_array pad_fields(const field_map &t_map) const;

    private:  /* Methods */
        string abbreviate(const string &t_data, const size_t &t_len = 35) const;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline std::ostream &operator<<(std::ostream &t_os, const SvcInfo &t_info)
    {
        return t_os << static_cast<std::string>(t_info);
    }
}

#endif // !SVC_INFO_H
