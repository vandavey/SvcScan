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
#include <string>
#include "../concepts/concepts.h"
#include "../errors/arg_ex.h"
#include "../errors/null_ptr_ex.h"
#include "../ranges/algo.h"
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
        static constexpr c_string_t EXE = "svcscan.exe";  // Executable name

        // Named argument flag alias regular expression pattern
        static constexpr c_string_t ALIAS_RGX = R"(^-[?\w]+$)";

        // Named argument flag regular expression pattern
        static constexpr c_string_t FLAG_RGX = R"(^--\w+(-*\w*)*$)";

        // Positional argument regular expression pattern
        static constexpr c_string_t POS_RGX = R"(^(?!-)[!-~\s]+$)";

        // Range notation regular expression pattern
        static constexpr c_string_t RANGE_RGX = R"(^\w+-\w+$)";

    public:  /* Fields */
        Args args;  // Command-line arguments

    private:  /* Fields */
        bool m_help_shown;    // Application help was shown
        bool m_valid;         // Arguments are valid

        string m_usage;       // Program usage message
        List<string> m_argv;  // Command-line argument list

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr ArgParser()
        {
            m_help_shown = m_valid = false;
            m_usage = algo::fstr("Usage: % [OPTIONS] TARGET", EXE);
        }

        ArgParser(const ArgParser&) = default;
        ArgParser(ArgParser&&) = default;

        virtual ~ArgParser() = default;

    public:  /* Operators */
        ArgParser& operator=(const ArgParser&) = default;
        ArgParser& operator=(ArgParser&&) = default;

    public:  /* Methods */
        /**
        * @brief
        *     Determine whether the extended application usage information was shown.
        */
        constexpr bool help_shown() const noexcept
        {
            return m_help_shown;
        }

        bool parse(int t_argc, char* t_argv[]);

    private:  /* Methods */
        /**
        * @brief
        *     Defragment the given command-line arguments so quoted
        *     string arguments are properly parsed and validated.
        */
        static constexpr List<string> defrag_argv(int t_argc, char* t_argv[])
        {
            if (t_argc < 1)
            {
                throw ArgEx{"t_argc", "Invalid argument count received"};
            }

            if (t_argv == nullptr)
            {
                throw NullPtrEx{"t_argv"};
            }

            List<string> defrag_list;
            const List<string> frag_list{algo::arg_vector(t_argc, t_argv)};

            // Defragment the given arguments
            for (size_t i{0_st}; i < frag_list.size(); i++)
            {
                const bool beg_quoted{frag_list[i].starts_with('\'')};

                if (!beg_quoted || (beg_quoted && frag_list[i].ends_with('\'')))
                {
                    defrag_list.push_back(frag_list[i]);
                    continue;
                }

                if (i == frag_list.size() - 1)
                {
                    defrag_list.push_back(frag_list[i]);
                    break;
                }

                // Locate terminating argument and parse the range
                for (size_t j{i + 1_st}; j < frag_list.size(); j++)
                {
                    if (frag_list[j].ends_with('\''))
                    {
                        defrag_list.push_back(frag_list.slice(i, j + 1_st).join(" "));
                        i = j;
                        break;
                    }
                }
            }

            return defrag_list;
        }

        /**
        * @brief
        *     Remove processed command-line arguments according to the given indexes.
        */
        constexpr void remove_processed_args(const vector<size_t>& t_indexes)
        {
            size_t delta{0_st};

            for (const size_t& index : algo::sort(t_indexes))
            {
                m_argv.remove_at(index - delta++);
            }
        }

        static bool is_alias(const string& t_arg);
        static bool is_flag(const string& t_arg);
        static bool is_port_range(const string& t_port);
        static bool is_value(const string& t_arg);

        bool error(const string& t_msg, bool t_valid = false);
        bool error(const string& t_arg, ArgType t_arg_type, bool t_valid = false);

        template<LShift T>
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
template<scan::LShift T>
inline bool scan::ArgParser::errorf(const string& t_msg, const T& t_arg, bool t_valid)
{
    std::cout << m_usage << LF;
    util::errorf(t_msg, t_arg);
    std::cout << LF;

    return m_valid = t_valid;
}

#endif // !SCAN_ARG_PARSER_H
