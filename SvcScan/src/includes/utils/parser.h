/*
*  parser.h
*  --------
*  Header file for command-line argument parser and validator
*/
#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <array>
#include <string>
#include <vector>
#include "../container/list.h"
#include "../container/range.h"
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

        using string    = std::string;
        using list_ui   = List<uint>;
        using list_s    = List<string>;
        using range_i   = Range<int>;
        using vector_s  = std::vector<string>;

        template<size_t N>
        using array_s = std::array<string, N>;

        static constexpr char EXE[] = "svcscan.exe";
        static constexpr char LF[]{ *Util::LF, '\0' };

    public:  /* Fields */
        static AutoProp<bool> verbose;  // Verbose output

        AutoProp<bool> help_shown;      // Usage was shown
        AutoProp<bool> valid;           // Arguments valid

        Property<string> addr;          // Target address
        Property<list_ui> ports;        // Target ports

    private:  /* Fields */
        string m_addr;   // 'addr' backing field
        string m_usage;  // Program usage

        list_s m_argv;    // Cmd-line arguments
        list_ui m_ports;  // 'ports' backing field

    public:  /* Constructors & Destructor */
        Parser(const int &t_argc, char *t_argv[]);

        virtual ~Parser() = default;

    private:  /* Constructors (deleted) */
        Parser() = delete;
        Parser(const Parser &) = delete;

    public:  /* Methods */
        void help();

    private:  /* Methods */
        void error(const string &t_arg,
                   const ArgType &t_arg_type,
                   const bool &t_valid = false);

        template<class T>
        void errorf(const string &t_msg,
                    const T &t_arg,
                    const bool &t_valid = false);

        void parse(const uint &t_argc, char *t_argv[]);
        void validate(list_s &t_list);

        bool parse_aliases(list_s &t_list);
        bool parse_flags(list_s &t_list);
        bool set_path(const string &t_path);
        bool set_ports(const string &t_ports);
        bool set_timeout(const string &t_ms);
    };
}

/// ***
/// Print usage and a formatted argument error to stderr
/// ***
template<class T>
inline void scan::Parser::errorf(const string &t_msg,
                                 const T &t_arg,
                                 const bool &t_valid) {
    std::cout << m_usage << LF;
    Util::errorf(t_msg, t_arg);
    std::cout << LF;

    valid = t_valid;
}

#endif // !PARSER_H
