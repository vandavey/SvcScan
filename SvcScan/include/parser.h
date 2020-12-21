#pragma once

#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>
#include <vector>
#include "autoprop.h"
#include "winclient.h"
#include "property.h"
#include "style.h"

#ifdef _WIN32
#  ifndef WIN_OS
#    define WIN_OS // Windows platform
#  endif// !WIN_OS
#else
#  ifndef NIX_OS
#    define NIX_OS // Unix platform
#  endif// !NIX_OS
#endif// _WIN32

namespace Scan
{
    /// ***
    /// Command-line argument parser and validator
    /// ***
    class Parser
    {
    private: /* Constants & Types */
        typedef unsigned int uint;
        typedef std::vector<std::string> vector_s;

        enum class ArgType : short { flag, optval, reqval };

    #ifdef WIN_OS
        static constexpr char EXE[] = "svcscan.exe";
    #else
        static constexpr char EXE[] = "svcscan";
    #endif// WIN_OS

    private: /* Fields */
        vector_s m_argv; // Cmd-line arguments
        vector_s m_ports; // Ports backing field

        std::string m_addr; // Address backing field
        std::string m_usage; // Program usage

    public: /* Properties */
        Property<vector_s> ports; // Target ports
        Property<std::string> addr; // Target address

        AutoProp<bool> help; // Show app usage info
        AutoProp<bool> valid; // Arguments valid

    public: /* Constructors & Destructor */
        Parser();
        Parser(const Parser &) = delete;
        Parser(const int &argc, const char *argv[]);

        virtual ~Parser();

    public: /* Methods */
        void show_help() const;

    private: /* Methods */
        void error(const std::string &arg, const ArgType &argt) const;
        void error(const std::string &msg, const std::string &arg) const;
        void parse(const uint &argc, const char *argv[]);
        void validate(const vector_s &arg);

        const int index(const char &flag, const std::string &arg) const;

        vector_s split(const std::string &data, const char &delim) const;
    };
}

#endif// !ARGPARSER_H
