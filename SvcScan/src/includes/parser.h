/*
*  parser.h
*  --------
*  Header file for command-line argument parser and validator
*/
#pragma once

#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>
#include <vector>
#include "properties/autoprop.h"
#include "properties/property.h"
#include "util.h"

namespace Scan
{
    /// ***
    /// Command-line argument parser and validator
    /// ***
    class Parser
    {
    private:  /* Constants & Types */
        typedef unsigned int uint;
        typedef std::vector<std::string> vector_s;

        enum class ArgType : short { flag, optval, reqval };

        static constexpr char EXE[] = "svcscan.exe";

    public:  /* Fields */
        Property<vector_s> ports;    // Target ports
        Property<std::string> addr;  // Target address

        AutoProp<bool> help_txt;     // Show app usage info
        AutoProp<bool> valid;        // Arguments valid

    private:  /* Fields */
        vector_s m_argv;      // Cmd-line arguments
        vector_s m_ports;     // Ports field

        std::string m_addr;   // Address field
        std::string m_usage;  // Program usage

    public:  /* Constructors & Destructor */
        Parser(const Parser &) = delete;
        Parser(const int &argc, const char *argv[]);
        virtual ~Parser() = default;

    private:  /* Constructors & Destructor */
        Parser();

    public:  /* Methods */
        void help() const;

    private:  /* Methods */
        void error(const std::string &arg, const ArgType &argt) const;
        void error(const std::string &msg, const std::string &arg) const;
        void parse(const uint &argc, const char *argv[]);
        void validate(const vector_s &arg);

        const int index(const char &flag, const std::string &arg) const;
    };
}

#endif // !ARGPARSER_H
