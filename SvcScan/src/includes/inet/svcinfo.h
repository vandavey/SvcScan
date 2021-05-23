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
#include "../properties/autoprop.h"
#include "../utils/parser.h"
#include "../utils/util.h"
#include "endpoint.h"
#include "hoststate.h"

namespace scan
{
    /// ***
    /// TCP network application service information
    /// ***
    class SvcInfo
    {
    private:  /* Types */
        using string   = std::string;
        using vector_s = std::vector<string>;

    public:  /* Fields */
        AutoProp<string> addr;      // Target address
        AutoProp<string> banner;    // Raw banner text
        AutoProp<string> info;      // Service information
        AutoProp<string> port;      // Target port
        AutoProp<string> proto;     // Protocol version
        AutoProp<string> service;   // Service name
        AutoProp<HostState> state;  // Target host state

    public:  /* Constructors & Destructor */
        SvcInfo() = default;
        SvcInfo(const SvcInfo &t_si);
        SvcInfo(const EndPoint &ep, const HostState &t_hs = HostState::unknown);

        SvcInfo(const EndPoint &t_ep, const string &t_banner,
                                      const HostState &t_hs = HostState::open);

        virtual ~SvcInfo() = default;

    public:  /* Operators */
        SvcInfo &operator=(const SvcInfo &t_si) noexcept;

    public:  /* Methods */
        void parse(const string &t_banner);

    private:  /* Methods */
        string shrink(const string &t_data, const size_t &t_len = 25) const;
        string upto_eol(const string &t_data) const;

        SvcInfo &swap(const SvcInfo &t_si) noexcept;
    };
}

#endif // !SVC_INFO_H
