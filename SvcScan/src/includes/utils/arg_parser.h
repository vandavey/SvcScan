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

namespace scan
{
    /**
    * @brief  Command-line argument parser and validator.
    */
    class ArgParser final
    {
    private:  /* Types & Type Aliases */
        enum class ArgType : short;

        using uint = unsigned int;

        using error_code = boost::system::error_code;
        using net        = NetUtil;
        using stdu       = StdUtil;
        using string     = std::string;

        template<class T>
        using vector = std::vector<T>;

    public:  /* Constants */
        static constexpr char REPO[] = "https://github.com/vandavey/SvcScan";

    private:  /* Constants */
        static constexpr char EXE[] = "svcscan.exe";  // Executable name
        static constexpr char LF[]  = "\n";           // EOL (line feed)

    public:  /* Fields */
        bool help_shown;  // Usage was shown
        bool valid;       // Arguments valid

        Args args;        // Command-line arguments

    private:  /* Fields */
        string m_usage;       // Program usage
        List<string> m_argv;  // Command-line argument list

    public:  /* Constructors & Destructor */
        ArgParser();
        ArgParser(const ArgParser &t_parser);
        ArgParser(ArgParser &&) = delete;

        virtual ~ArgParser() = default;

    public:  /* Operators */
        ArgParser &operator=(const ArgParser &) = default;
        ArgParser &operator=(ArgParser &&) = default;

    public:  /* Methods */
        bool help();
        bool parse_argv(const int &t_argc, char *t_argv[]);

    private:  /* Methods */
        static bool is_port_range(const string &t_port);

        bool error(const string &t_arg,
                   const ArgType &t_arg_type,
                   const bool &t_valid = false);

        template<class T>
        bool errorf(const string &t_msg,
                    const T &t_arg,
                    const bool &t_valid = false);

        bool parse_aliases(List<string> &t_list);
        bool parse_flags(List<string> &t_list);
        bool set_path(const string &t_path);
        bool set_ports(const string &t_ports);
        bool set_timeout(const string &t_ms);
        bool set_uri(const string &t_uri);
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
    std::cout << m_usage << LF;
    stdu::errorf(t_msg, t_arg);
    std::cout << LF;

    return (valid = t_valid);
}

#endif // !ARG_PARSER_H
