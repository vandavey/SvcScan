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

namespace Scan
{
    /// ***
    /// TCP network application service information
    /// ***
    class SvcInfo
    {
    private:  /* Types */
        using string = std::string;
        using vector_s = std::vector<string>;

    public:  /* Fields */
        AutoProp<string> addr;      // Target address
        AutoProp<string> banner;    // Raw banner text
        AutoProp<string> port;      // Target port
        AutoProp<string> proto;     // Protocol version
        AutoProp<string> service;   // Service name
        AutoProp<HostState> state;  // Target host state
        AutoProp<string> version;   // Service version

    public:  /* Constructors & Destructor */
        SvcInfo() = default;
        SvcInfo(const SvcInfo &si);
        SvcInfo(const EndPoint &ep, const HostState &hs = HostState::unknown);

        SvcInfo(const EndPoint &ep, const string &banner,
                                    const HostState &hs = HostState::open);

        virtual ~SvcInfo() = default;

    public:  /* Operators */
        SvcInfo &operator=(const SvcInfo &si) noexcept;

    public:  /* Methods */
        void parse(const string &banner_txt);

    private:  /* Methods */
        const string upto_eol(const string &data) const;

        SvcInfo &swap(const SvcInfo &si) noexcept;
    };
}

#endif // !SVC_INFO_H
