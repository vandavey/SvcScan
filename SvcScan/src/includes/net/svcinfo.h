/*
*  svcinfo.h
*  ---------
*  Header file for TCP network application information
*/
#pragma once

#ifndef SVCINFO_H
#define SVCINFO_H

#include <string>
#include "../net/endpoint.h"
#include "../properties/autoprop.h"
#include "../util.h"

namespace Scan
{
    /// ***
    /// TCP network application service information
    /// ***
    class SvcInfo
    {
    public:  /* Fields */
        AutoProp<EndPoint> ep;          // Connection endpoint
        AutoProp<std::string> proto;    // Protocol version
        AutoProp<std::string> service;  // Service name
        AutoProp<std::string> version;  // Service version

    private:  /* Fields */
        std::string m_banner;  // Raw banner text

    public:  /* Constructors & Destructor */
        SvcInfo() = default;
        explicit SvcInfo(const SvcInfo &si);
        explicit SvcInfo(const EndPoint &ep);
        SvcInfo(const EndPoint &ep, const std::string &banner);

        virtual ~SvcInfo() = default;

    public:  /* Operators */
        SvcInfo &operator=(const std::string &banner);
        SvcInfo &operator=(const SvcInfo &si);

        friend std::ostream &operator<<(std::ostream &os, SvcInfo &si);

    public:  /* Methods */
        const std::string upto_eol(const std::string &data) const;

        SvcInfo &parse(const std::string &banner);
        SvcInfo &swap(const SvcInfo &si);
    };

    /// ***
    /// Bitwise left shift friend operator definition
    /// ***
    inline std::ostream &operator<<(std::ostream &os, SvcInfo &si)
    {
        // Append protocol versison to service
        if (!si.service.get().empty())
        {
            si.service += (std::string(" (") + si.proto.get() + ")");
        }
        const std::string header(si.ep.get().str());

        os << header << Util::LF
            << std::string(header.length(), '-') << Util::LF
            << "Port    : " << si.ep.get().port << "/tcp" << Util::LF
            << "Service : " << si.service << Util::LF
            << "Version : " << si.version << Util::LF;

        /**
        * TODO: Add verbose output option
        **/
        // Show raw banner text
        if (si.service.get().empty())
        {
            os << "Banner  : " << si.m_banner << Util::LF;
        }
        return os;
    }
}

#endif // !SVCINFO_H
