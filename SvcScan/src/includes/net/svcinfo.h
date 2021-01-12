/*
*  svcinfo.h
*  ---------
*  Header file for TCP network application information
*/
#pragma once

#ifndef SVCINFO_H
#define SVCINFO_H

#include <string>
#include "../properties/autoprop.h"
#include "../net/endpoint.h"
#include "../util.h"

namespace Scan
{
    /// ***
    /// TCP network application service information
    /// ***
    class SvcInfo
    {
    public: /* Properties */
        AutoProp<EndPoint> ep; // Connection endpoint

        AutoProp<std::string> proto; // Protocol version
        AutoProp<std::string> service; // Service name
        AutoProp<std::string> version; // Service version

    public: /* Constructors & Destructor */
        SvcInfo();
        SvcInfo(const SvcInfo &si);
        SvcInfo(const EndPoint &ep);
        SvcInfo(const EndPoint &ep, const std::string &banner);

        virtual ~SvcInfo();

    public: /* Operators */
        SvcInfo &operator=(const std::string &banner);
        SvcInfo &operator=(const SvcInfo &si);

        friend std::ostream &operator<<(std::ostream &os, SvcInfo &si);

    public: /* Methods */
        std::string upto_eol(const std::string &data) const;

        SvcInfo &parse(std::string &banner);
        SvcInfo &swap(const SvcInfo &si);
    };

    /// ***
    /// Bitwise left shift friend operator definition
    /// ***
    inline std::ostream &operator<<(std::ostream &os, SvcInfo &si)
    {
        const std::string header = {si.ep.get().str()};
        (si.service += " (") += si.proto + ")";

        os << header << std::endl
            << std::string(header.length(), '-') << std::endl
            << "Port: " << si.ep.get().port << "/tcp" << std::endl
            << "Service: " << si.service << std::endl
            << "Version: " << si.version << std::endl;

        return os;
    }
}

#endif // !SVCINFO_H
