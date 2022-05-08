/*
*  parser.h
*  --------
*  Header file for command-line argument parser and validator
*/
#pragma once

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "../inet/netutil.h"
#include "../io/stdutil.h"
#include "args.h"

namespace scan
{
    /// ***
    /// Command-line argument parser and validator
    /// ***
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
        static constexpr char EXE[] = "svcscan.exe";       // Executable name
        static constexpr char LF[]{ *StdUtil::LF, '\0' };  // EOL (line feed)

    public:  /* Fields */
        Args args;        // Cmd-line arguments

        bool help_shown;  // Usage was shown
        bool valid;       // Arguments valid

    private:  /* Fields */
        string m_usage;       // Program usage
        List<string> m_argv;  // Cmd-line argument list

    public:  /* Constructors & Destructor */
        ArgParser();
        ArgParser(const ArgParser &) = delete;

        virtual ~ArgParser() = default;

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

/// ***
/// Print usage and a formatted argument error to stderr
/// ***
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
