/*
*  parser.h
*  --------
*  Header file for command-line argument parser and validator
*/
#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "../container/list.h"
#include "../properties/autoprop.h"
#include "util.h"

namespace Scan
{
    /// ***
    /// Command-line argument parser and validator
    /// ***
    class Parser
    {
    private:  /* Types & Constants */
        using uint = unsigned int;

        using string = std::string;
        using list_s = List<string>;
        using vector_s = std::vector<string>;

        enum class ArgType : short;

        static constexpr char EXE[] = "svcscan.exe";
        static constexpr char LF[] = {*Util::LF};

    public:  /* Fields */
        static AutoProp<bool> verbose;  // Verbose output

        AutoProp<bool> help_shown;      // Usage was shown
        AutoProp<bool> valid;           // Arguments valid

        Property<string> addr;          // Target address
        Property<list_s> ports;         // Target ports

    private:  /* Fields */
        std::string m_addr;   // 'addr' backing field
        std::string m_usage;  // Program usage

        list_s m_argv;        // Cmd-line arguments
        list_s m_ports;       // 'ports' backing field

    public:  /* Constructors & Destructor */
        Parser(const int &argc, const char *argv[]);
        virtual ~Parser() = default;

    private:  /* Constructors (deleted) */
        Parser() = delete;
        Parser(const Parser &) = delete;

    public:  /* Methods */
        void help();

    private:  /* Methods */
        void error(const std::string &arg, const ArgType &arg_type) const;
        void errorf(const std::string &msg, const std::string &arg) const;
        void parse(const uint &argc, const char *argv[]);
        void validate(list_s &list);

        const bool parse_aliases(list_s &list);
        const bool parse_flags(list_s &list);
        const bool parse_ports(const std::string &ports);
    };
}

#endif // !PARSER_H
