/*
*  svcinfo.h
*  ---------
*  Header file for network application information
*/
#pragma once

#ifndef SVC_INFO_H
#define SVC_INFO_H

#include "../../io/stdutil.h"
#include "endpoint.h"
#include "hoststate.h"

namespace scan
{
    /// ***
    /// TCP network application service information
    /// ***
    class SvcInfo
    {
    private:  /* Type Aliases */
        using stdu   = StdUtil;
        using string = std::string;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Fields */
        HostState state;  // Target host state

        string addr;      // Target address
        string banner;    // Raw banner text
        string port;      // Target port
        string proto;     // Protocol version
        string service;   // Service name
        string summary;   // Service summary

    public:  /* Constructors & Destructor */
        SvcInfo();
        SvcInfo(const SvcInfo &t_si);
        SvcInfo(const Endpoint &ep, const HostState &t_hs = HostState::unknown);

        SvcInfo(const Endpoint &t_ep,
                const string &t_banner,
                const HostState &t_hs = HostState::open);

        virtual ~SvcInfo() = default;

    public:  /* Operators */
        SvcInfo &operator=(const SvcInfo &t_si) noexcept;

    public:  /* Methods */
        void parse(const string &t_banner);
        void reset(const string &t_addr = string());

    private:  /* Methods */
        string shrink(const string &t_data, const size_t &t_len = 35) const;
        string upto_last_eol(const string &t_data) const;
    };
}

#endif // !SVC_INFO_H
