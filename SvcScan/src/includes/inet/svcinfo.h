/*
*  svcinfo.h
*  ---------
*  Header file for network application information
*/
#pragma once

#ifndef SVC_INFO_H
#define SVC_INFO_H

#include <string>
#include <vector>
#include "../io/stdutil.h"
#include "../utils/util.h"
#include "hoststate.h"
#include "sockets/endpoint.h"

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

        using vector_s = std::vector<string>;

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

    private:  /* Methods */
        string shrink(const string &t_data, const size_t &t_len = 25) const;
        string upto_last_eol(const string &t_data) const;
    };
}

#endif // !SVC_INFO_H
