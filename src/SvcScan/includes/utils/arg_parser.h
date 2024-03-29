/*
*  arg_parser.h
*  ------------
*  Header file for a command-line argument parser and validator
*/
#pragma once

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "../inet/net_util.h"
#include "../io/std_util.h"
#include "args.h"
#include "type_defs.h"

namespace scan
{
    /**
    * @brief  Command-line argument parser and validator.
    */
    class ArgParser final
    {
    private:  /* Types & Type Aliases */
        using algo = Algorithm;
        using net  = NetUtil;
        using stdu = StdUtil;

        enum class ArgType : byte_t;

    private:  /* Constants */
        static constexpr cstr_t<12> EXE = { "svcscan.exe" };  // Executable name

        // Named argument regular expression patterns
        static constexpr cstr_t<14> ALIAS_RGX = { "^-[A-Z?a-z]+$" };
        static constexpr cstr_t<14> FLAG_RGX  = { "^--[A-Za-z]+$" };

        // Positional argument regular expression pattern
        static constexpr cstr_t<29> POS_RGX = { R"(^(?!-)[\\/0-9,\-A-Z\.a-z~]+$)" };

    public:  /* Fields */
        Args args;  // Command-line arguments

    private:  /* Fields */
        bool m_help_shown;    // Application help was shown
        bool m_valid;         // Arguments are valid

        string m_usage;       // Program usage message
        List<string> m_argv;  // Command-line argument list

    public:  /* Constructors & Destructor */
        ArgParser();
        ArgParser(const ArgParser &t_parser) noexcept;
        ArgParser(ArgParser &&) = delete;

        virtual ~ArgParser() = default;

    public:  /* Operators */
        ArgParser &operator=(const ArgParser &) = default;
        ArgParser &operator=(ArgParser &&) = default;

    public:  /* Methods */
        static string app_title(const string &t_word_sep = { });

        bool help();
        bool help_shown() const noexcept;
        bool parse_argv(const int &t_argc, char *t_argv[]);

    private:  /* Methods */
        static bool is_alias(const string &t_arg);
        static bool is_flag(const string &t_arg);
        static bool is_port_range(const string &t_port);
        static bool is_value(const string &t_arg);

        static List<string> defrag_argv(const int &t_argc, char *t_argv[]);

        void remove_processed_args(const vector<size_t> &t_indexes);

        bool error(const string &t_msg, const bool &t_valid = false);

        bool error(const string &t_arg,
                   const ArgType &t_arg_type,
                   const bool &t_valid = false);

        template<class T>
        bool errorf(const string &t_msg,
                    const T &t_arg,
                    const bool &t_valid = false);

        bool parse_aliases(List<string> &t_list);

        bool parse_curl_uri(const IndexPair<string> &t_pair,
                            List<size_t> &t_proc_indexes);

        bool parse_flags(List<string> &t_list);

        bool parse_path(const IndexPair<string> &t_pair,
                        List<size_t> &t_proc_indexes);

        bool parse_port_range(const string &t_ports);
        bool parse_ports(const string &t_ports);

        bool parse_ports(const IndexPair<string> &t_pair,
                         List<size_t> &t_proc_indexes);

        bool parse_threads(const IndexPair<string> &t_pair,
                           List<size_t> &t_proc_indexes);

        bool parse_timeout(const IndexPair<string> &t_pair,
                           List<size_t> &t_proc_indexes);

        bool validate(List<string> &t_list);

        string error(const error_code &t_ecode);
    };
}

/**
* @brief  Write the application usage information and an interpolated
*         error message to the standard error stream.
*/
template<class T>
inline bool scan::ArgParser::errorf(const string &t_msg,
                                    const T &t_arg,
                                    const bool &t_valid) {
    std::cout << m_usage << &LF[0];
    stdu::errorf(t_msg, t_arg);
    std::cout << &LF[0];

    return m_valid = t_valid;
}

#endif // !ARG_PARSER_H
