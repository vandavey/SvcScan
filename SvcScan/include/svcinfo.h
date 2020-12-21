#pragma once

#ifndef SVCINFO_H
#define SVCINFO_H

#include <string>
#include "autoprop.h"
#include "endpoint.h"

namespace Scan
{
    /// ***
    /// TCP application service information
    /// ***
    class SvcInfo
    {
    private: /* Constants */
        static constexpr char CR = {'\r'}; // Carriage-return 
        static constexpr char LF = {'\n'}; // Unix style EOL
        static constexpr char CRLF[] = {CR, LF}; // NT style EOL

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
        SvcInfo &operator=(std::string &banner);
        SvcInfo &operator=(const SvcInfo &si);

        friend std::ostream &operator<<(std::ostream &os, SvcInfo &si);

    public: /* Methods */
        std::string upto_eol(const std::string &data) const;

        SvcInfo &parse(std::string &banner);
        SvcInfo &swap(const SvcInfo &si);
    };

    /// ***
    /// Left shift friend operator definitions
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

#endif// !SVCINFO_H
