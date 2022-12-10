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

    public:  /* Fields */
        Args args;  // Command-line arguments

    private:  /* Fields */
        bool m_help_shown;    // App usage was shown
        bool m_valid;         // Arguments valid

        string m_usage;       // Program usage
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

        bool error(const string &t_arg,
                   const ArgType &t_arg_type,
                   const bool &t_valid = false);

        template<class T>
        bool errorf(const string &t_msg,
                    const T &t_arg,
                    const bool &t_valid = false);

        bool parse_aliases(List<string> &t_list);
        bool parse_flags(List<string> &t_list);
        bool set_curl_uri(const string &t_uri, const bool &t_remove_arg);
        bool set_path(const string &t_path);
        bool set_port_range(const string &t_ports);
        bool set_ports(const string &t_ports);
        bool set_threads(const string &t_threads);
        bool set_timeout(const string &t_ms);
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
