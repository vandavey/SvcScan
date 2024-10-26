/*
* @file
*     arg_parser.h
* @brief
*     Header file for a command-line argument parser and validator.
*/
#pragma once

#ifndef SCAN_ARG_PARSER_H
#define SCAN_ARG_PARSER_H

#include <cstdint>
#include <iostream>
#include "../ranges/list.h"
#include "../utils/aliases.h"
#include "../utils/const_defs.h"
#include "args.h"
#include "indexed_arg.h"
#include "util.h"

namespace scan
{
    /**
    * @brief
    *     Command-line argument parser and validator.
    */
    class ArgParser final
    {
    private:  /* Types & Type Aliases */
        enum class ArgType : uint8_t;

    private:  /* Constants */
        static constexpr cstr_t EXE = "svcscan.exe";  // Executable name

        // Named argument flag alias regular expression pattern
        static constexpr cstr_t ALIAS_RGX = R"(^-[?\w]+$)";

        // Named argument flag regular expression pattern
        static constexpr cstr_t FLAG_RGX = R"(^--\w+(-*\w*)*$)";

        // Positional argument regular expression pattern
        static constexpr cstr_t POS_RGX = R"(^(?!-)[!-~\s]+$)";

        // Range notation regular expression pattern
        static constexpr cstr_t RANGE_RGX = R"(^\w+-\w+$)";

    public:  /* Fields */
        Args args;  // Command-line arguments

    private:  /* Fields */
        bool m_help_shown;    // Application help was shown
        bool m_valid;         // Arguments are valid

        string m_usage;       // Program usage message
        List<string> m_argv;  // Command-line argument list

    public:  /* Constructors & Destructor */
        ArgParser();
        ArgParser(const ArgParser&) = default;
        ArgParser(ArgParser&&) = default;

        virtual ~ArgParser() = default;

    public:  /* Operators */
        ArgParser& operator=(const ArgParser&) = default;
        ArgParser& operator=(ArgParser&&) = default;

    public:  /* Methods */
        bool help_shown() const noexcept;
        bool parse(int t_argc, char* t_argv[]);

    private:  /* Methods */
        static bool is_alias(const string& t_arg);
        static bool is_flag(const string& t_arg);
        static bool is_port_range(const string& t_port);
        static bool is_value(const string& t_arg);

        static List<string> defrag_argv(int t_argc, char* t_argv[]);

        void remove_processed_args(const vector<size_t>& t_indexes);

        bool error(const string& t_msg, bool t_valid = false);
        bool error(const string& t_arg, ArgType t_arg_type, bool t_valid = false);

        template<class T>
        bool errorf(const string& t_msg, const T& t_arg, bool t_valid = false);

        bool help();
        bool parse_aliases(List<string>& t_list);

        bool parse_curl_uri(const IndexedArg& t_indexed_arg,
                            List<size_t>& t_proc_indexes);

        bool parse_flags(List<string>& t_list);
        bool parse_path(const IndexedArg& t_indexed_arg, List<size_t>& t_proc_indexes);
        bool parse_port_range(const string& t_ports);
        bool parse_ports(const string& t_ports);
        bool parse_ports(const IndexedArg& t_indexed_arg, List<size_t>& t_proc_indexes);
        bool parse_threads(const IndexedArg& t_indexed_arg, List<size_t>& t_proc_indexes);
        bool parse_timeout(const IndexedArg& t_indexed_arg, List<size_t>& t_proc_indexes);
        bool validate(List<string>& t_list);

        string error(const error_code& t_ecode);
    };
}

/**
* @brief
*     Write the application usage information and an
*     interpolated error message to the standard error stream.
*/
template<class T>
inline bool scan::ArgParser::errorf(const string& t_msg, const T& t_arg, bool t_valid)
{
    std::cout << m_usage << LF;
    util::errorf(t_msg, t_arg);
    std::cout << LF;

    return m_valid = t_valid;
}

#endif // !SCAN_ARG_PARSER_H
