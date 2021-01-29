/*
*  svcinfo.h
*  ---------
*  Header file for TCP network application information
*/
#pragma once

#ifndef SVC_INFO_H
#define SVC_INFO_H

#include <string>
#include "../net/endpoint.h"
#include "../properties/autoprop.h"
#include "../parser.h"
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
        SvcInfo &parse(const std::string &banner);

    private:  /* Methods */
        const std::string upto_eol(const std::string &data) const;

        SvcInfo &swap(const SvcInfo &si) noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &os, SvcInfo &si)
    {
        // Append protocol version to service
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

        // Skip banner output
        if (!Parser::verbose.get() && !si.service.get().empty())
        {
            return os;
        }
        os << "Banner  : \"";

        // Show full banner text
        if (Parser::verbose.get())
        {
            return (os << si.m_banner << "\"" << Util::LF);
        }

        // Show first 35 characters of banner
        for (size_t i = {0}; i < si.m_banner.size(); i++)
        {
            if ((i > 35) || (i == (si.m_banner.size() - 1)))
            {
                // Indicate banner text was shortened
                if (i > 35)
                {
                    os << "...";
                }

                os << "\"";
                break;
            }
            os << si.m_banner[i];
        }
        return (os << Util::LF);
    }
}

#endif // !SVC_INFO_H
