/*
*  parser.h
*  --------
*  Header file for command-line argument parser and validator
*/
#pragma once

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <array>
#include <string>
#include <vector>
#include "../conio/stdutil.h"
#include "../containers/generic/list.h"
#include "../containers/generic/range.h"
#include "../inet/netutil.h"
#include "../properties/autoprop.h"

namespace scan
{
    /// ***
    /// Command-line argument parser and validator
    /// ***
    class ArgParser final
    {
    private:  /* Types */
        enum class ArgType : short;

        using uint = unsigned int;

        using net    = NetUtil;
        using stdu   = StdUtil;
        using string = std::string;

        using list_s   = List<string>;
        using list_ui  = List<uint>;
        using range_i  = Range<int>;
        using vector_s = std::vector<string>;

        template<size_t N>
        using array_s = std::array<string, N>;

    public:  /* Constants */
        static constexpr char REPO[] = "https://github.com/vandavey/SvcScan";

    private:  /* Constants */
        static constexpr char EXE[] = "svcscan.exe";       // Executable name
        static constexpr char LF[]{ *StdUtil::LF, '\0' };  // EOL (line feed)

    public:  /* Fields */
        static AutoProp<bool> verbose;  // Verbose output

        AutoProp<bool> help_shown;      // Usage was shown
        AutoProp<bool> valid;           // Arguments valid

        Property<string> addr;          // Target address
        Property<list_ui> ports;        // Target ports

    private:  /* Fields */
        string m_addr;    // 'addr' backing field
        string m_usage;   // Program usage

        list_s m_argv;    // Cmd-line arguments
        list_ui m_ports;  // 'ports' backing field

    public:  /* Constructors & Destructor */
        ArgParser();

        virtual ~ArgParser() = default;

    private:  /* Constructors (deleted) */
        ArgParser(const ArgParser &) = delete;

    public:  /* Methods */
        bool help();
        bool parse_argv(const int &t_argc, char *t_argv[]);

    private:  /* Methods */
        bool error(const string &t_arg,
                   const ArgType &t_arg_type,
                   const bool &t_valid = false);

        template<class T>
        bool errorf(const string &t_msg,
                    const T &t_arg,
                    const bool &t_valid = false);

        bool parse_aliases(list_s &t_list);
        bool parse_flags(list_s &t_list);
        bool set_path(const string &t_path);
        bool set_ports(const string &t_ports);
        bool set_timeout(const string &t_ms);
        bool validate(list_s &t_list);
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
