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

namespace scan
{
    /// ***
    /// Command-line argument parser and validator
    /// ***
    class Parser
    {
    private:  /* Types & Constants */
        enum class ArgType : short;

        using uint = unsigned int;

        using string = std::string;
        using list_s = List<string>;
        using vector_s = std::vector<string>;

        static constexpr char EXE[] = "svcscan.exe";
        static constexpr char LF[]{ *Util::LF, '\0' };

    public:  /* Fields */
        static AutoProp<bool> verbose;  // Verbose output

        AutoProp<bool> help_shown;      // Usage was shown
        AutoProp<bool> valid;           // Arguments valid

        Property<string> addr;          // Target address
        Property<list_s> ports;         // Target ports

    private:  /* Fields */
        string m_addr;   // 'addr' backing field
        string m_usage;  // Program usage

        list_s m_argv;   // Cmd-line arguments
        list_s m_ports;  // 'ports' backing field

    public:  /* Constructors & Destructor */
        Parser(const int &t_argc, char *t_argv[]);
        virtual ~Parser() = default;

    private:  /* Constructors (deleted) */
        Parser() = delete;
        Parser(const Parser &) = delete;

    public:  /* Methods */
        void help();

    private:  /* Methods */
        void error(const string &t_arg, const ArgType &t_arg_type) const;
        void errorf(const string &t_msg, const string &t_arg) const;
        void parse(const uint &t_argc, char *t_argv[]);
        void validate(list_s &t_list);

        bool parse_aliases(list_s &t_list);
        bool parse_flags(list_s &t_list);
        bool parse_ports(const string &t_ports);
    };
}

#endif // !PARSER_H
