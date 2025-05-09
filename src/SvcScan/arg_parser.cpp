/*
* @file
*     arg_parser.cpp
* @brief
*     Source file for a command-line argument parser and validator.
*/
#include <array>
#include <boost/asio/error.hpp>
#include "includes/console/arg_parser.h"
#include "includes/file_system/file.h"
#include "includes/file_system/path.h"
#include "includes/file_system/path_info.h"
#include "includes/inet/http/request.h"
#include "includes/inet/net.h"
#include "includes/inet/net_const_defs.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Command-line argument enumeration.
*/
enum class scan::ArgParser::ArgType : uint8_t
{
    unknown,  // Unknown argument type
    alias,    // Argument alias (e.g., -f)
    flag,     // Argument flag (e.g., --foo)
    value     // Positional or named argument value
};

/**
* @brief
*     Parse and validate the raw command-line arguments.
*/
bool scan::ArgParser::parse(int t_argc, char* t_argv[])
{
    if (t_argv == nullptr)
    {
        throw NullPtrEx{"t_argv"};
    }
    bool show_help{false};

    args.exe_path = path::normalize(t_argv[0]);
    m_argv = args.argv = defrag_argv(t_argc, t_argv);

    // Display program usage and stop validation
    if (t_argc == 1 || m_argv.any("-?", "-h", "--help"))
    {
        show_help = true;
    }

    return show_help ? help() : validate(m_argv);
}

/**
* @brief
*     Determine whether the given command-line argument is an argument alias.
*/
bool scan::ArgParser::is_alias(const string& t_arg)
{
    return algo::matches(t_arg, ALIAS_RGX);
}

/**
* @brief
*     Determine whether the given command-line argument is an argument flag.
*/
bool scan::ArgParser::is_flag(const string& t_arg)
{
    return algo::matches(t_arg, FLAG_RGX);
}

/**
* @brief
*     Determine whether the given port is in range notation (e.g., n1-n2).
*     Do not yet check whether range bounds are integral.
*/
bool scan::ArgParser::is_port_range(const string& t_port)
{
    return algo::matches(t_port, RANGE_RGX);
}

/**
* @brief
*     Determine whether the given command-line argument is a positional argument
*     or the value argument for an argument flag or an argument alias.
*/
bool scan::ArgParser::is_value(const string& t_arg)
{
    return algo::matches(t_arg, POS_RGX);
}

/**
* @brief
*     Write the application usage information and an
*     error message to the standard error stream.
*/
bool scan::ArgParser::error(const string& t_msg, bool t_valid)
{
    std::cout << usage() << LF;
    util::errorf(t_msg);
    std::cout << LF;

    return m_valid = t_valid;
}

/**
* @brief
*     Write the application usage information and a command-line
*     argument error message to the standard error stream.
*/
bool scan::ArgParser::error(const string& t_arg, ArgType t_type, bool t_valid)
{
    bool valid;

    switch (t_type)
    {
        case ArgType::unknown:
            valid = errorf("Unable to validate argument: '%'", t_arg, t_valid);
            break;
        case ArgType::alias:
            valid = errorf("Missing value for argument alias: '%'", t_arg, t_valid);
            break;
        case ArgType::flag:
            valid = errorf("Missing value for argument flag: '%'", t_arg, t_valid);
            break;
        case ArgType::value:
            valid = errorf("Missing required argument(s): '%'", t_arg, t_valid);
            break;
        default:
            valid = t_valid;
            break;
    }
    return valid;
}

/**
* @brief
*     Write the extended application usage information to the standard output
*     stream. Returns false to indicate that no argument parsing errors occurred.
*/
bool scan::ArgParser::help()
{
    m_help_shown = true;

    const List<string> usage_lines
    {
        util::app_title(),
        usage() + LF,
        "Network service scanner application\n",
        "Positional Arguments:",
        "  TARGET                      Target IPv4 address or hostname\n",
        "Named Arguments:",
        "  -h/-?,     --help           Show this help message and exit",
        "  -v,        --verbose        Enable verbose console output",
        "  -s,        --ssl            Enable SSL/TLS socket connections",
        "  -j,        --json           Output scan results in JSON format",
        "  -p PORT,   --port PORT      Port number(s) - comma separated (no spaces)",
        "  -t MS,     --timeout MS     Connection timeout (milliseconds)",
        "                              [ Default: 3500 ]",
        "  -T NUM,    --threads NUM    Thread pool size (execution thread count)",
        "                              [ Default: system thread count or 16 ]",
        "  -o PATH,   --output PATH    Write the scan results to a file",
        "  -c [URI],  --curl [URI]     Use GET requests for HTTP/HTTPS probing\n",
        "Usage Examples:",
        "  svcscan.exe -v localhost 21,443,80",
        "  svcscan.exe -p 22-25,53 192.168.1.1",
        "  svcscan.exe -vt 500 192.168.1.1 4444",
        "  svcscan.exe --curl /admin 192.168.1.1 80",
        "  svcscan.exe --ssl 192.168.1.1 443 --curl",
    };

    std::cout << algo::concat(usage_lines.join_lines(), LF, LF);
    return false;
}

/**
* @brief
*     Parse and validate the given command-line argument flag
*     aliases and their corresponding arguments (e.g., -f foo).
*/
bool scan::ArgParser::parse_aliases(List<string>& t_list)
{
    bool valid{true};
    List<size_t> proc_indexes;

    for (const IndexedArg& indexed_alias : algo::enumerate(t_list, ALIAS_RGX))
    {
        for (const char& ch : indexed_alias.value)
        {
            switch (ch)
            {
                case CHAR_DASH:
                    continue;
                case '?':
                case 'h':
                    valid = help();
                    break;
                case 'v':
                    args.verbose = true;
                    break;
                case 's':
                    args.tls_enabled = true;
                    break;
                case 'j':
                    args.out_json = true;
                    break;
                case 'p':
                    valid = parse_ports(indexed_alias, proc_indexes);
                    break;
                case 't':
                    valid = parse_timeout(indexed_alias, proc_indexes);
                    break;
                case 'T':
                    valid = parse_threads(indexed_alias, proc_indexes);
                    break;
                case 'o':
                    valid = parse_path(indexed_alias, proc_indexes);
                    break;
                case 'c':
                    valid = parse_curl_uri(indexed_alias, proc_indexes);
                    break;
                default:
                    valid = errorf("Unrecognized flag: '-%'", ch);
                    break;
            }

            if (!valid)
            {
                break;
            }
        }

        if (!valid)
        {
            break;
        }
        proc_indexes.emplace_back(indexed_alias.index);
    }
    remove_processed_args(proc_indexes);

    return valid;
}

/**
* @brief
*     Parse and validate the HTTP request URI argument
*     the underlying command-line arguments list.
*/
bool scan::ArgParser::parse_curl_uri(const IndexedArg& t_indexed_arg,
                                     List<size_t>& t_proc_indexes)
{
    if (!m_argv.valid_index(t_indexed_arg.index))
    {
        throw ArgEx{INVALID_ARG_INDEX_MSG, "t_indexed_arg"};
    }
    args.curl = true;

    bool valid{true};
    const size_t value_index{t_indexed_arg.index + 1_sz};

    if (m_argv.valid_index(value_index) && is_value(m_argv[value_index]))
    {
        const string uri{m_argv[value_index]};

        if (Request<>::valid_uri(uri))
        {
            args.uri = uri;
            t_proc_indexes.emplace_back(value_index);
        }
        else  // Invalid URI was received
        {
            valid = errorf("'%' is not a valid HTTP URI", uri);
        }
    }
    return valid;
}

/**
* @brief
*     Parse and validate the given command-line argument flags
*     and their corresponding arguments (e.g., --flag foo).
*/
bool scan::ArgParser::parse_flags(List<string>& t_list)
{
    if (t_list.contains("--"))
    {
        return error("--", ArgType::unknown);
    }

    bool valid{true};
    List<size_t> proc_indexes;

    for (const IndexedArg& indexed_flag : algo::enumerate(t_list, FLAG_RGX))
    {
        if (indexed_flag.value == "--help")
        {
            valid = help();
        }
        else if (indexed_flag.value == "--verbose")
        {
            args.verbose = true;
        }
        else if (indexed_flag.value == "--json")
        {
            args.out_json = true;
        }
        else if (indexed_flag.value == "--ssl")
        {
            args.tls_enabled = true;
        }
        else if (indexed_flag.value == "--port")
        {
            valid = parse_ports(indexed_flag, proc_indexes);
        }
        else if (indexed_flag.value == "--timeout")
        {
            valid = parse_timeout(indexed_flag, proc_indexes);
        }
        else if (indexed_flag.value == "--threads")
        {
            valid = parse_threads(indexed_flag, proc_indexes);
        }
        else if (indexed_flag.value == "--output")
        {
            valid = parse_path(indexed_flag, proc_indexes);
        }
        else if (indexed_flag.value == "--curl")
        {
            valid = parse_curl_uri(indexed_flag, proc_indexes);
        }
        else  // Unknown flag name
        {
            valid = errorf("Unrecognized flag: '%'", indexed_flag.value);
        }

        if (!valid)
        {
            break;
        }
        proc_indexes.emplace_back(indexed_flag.index);
    }
    remove_processed_args(proc_indexes);

    return valid;
}

/**
* @brief
*     Parse and validate the given report output path and
*     update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_path(const IndexedArg& t_indexed_arg,
                                 List<size_t>& t_proc_indexes)
{
    if (!m_argv.valid_index(t_indexed_arg.index))
    {
        throw ArgEx{INVALID_ARG_INDEX_MSG, "t_indexed_arg"};
    }

    bool valid{true};
    const size_t value_index{t_indexed_arg.index + 1_sz};

    if (m_argv.valid_index(value_index) && is_value(m_argv[value_index]))
    {
        const string path{path::resolve(m_argv[value_index])};

        switch (path::path_info(path))
        {
            case PathInfo::empty:
                valid = errorf("Output file path cannot be empty", path);
                break;
            case PathInfo::directory:
                valid = errorf("File path leads to a directory: '%'", path);
                break;
            case PathInfo::new_directory:
                valid = errorf("File path leads to a new directory: '%'", path);
                break;
            case PathInfo::not_found:
                valid = errorf("Parent path not found: '%'", path::parent(path));
                break;
            case PathInfo::file:
            case PathInfo::new_file:
                args.out_path = path;
                t_proc_indexes.emplace_back(value_index);
                break;
            default:
                valid = errorf("Invalid output file path: '%'", path);
                break;
        }
    }
    else  // Missing value argument
    {
        valid = error("-o/--output PATH", ArgType::flag);
    }

    // Inaccessible report path
    if (valid && !File::touch(args.out_path))
    {
        valid = errorf("Inaccessible output path: '%'", args.out_path);
    }
    return valid;
}

/**
* @brief
*     Parse and validate the given port range and
*     update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_port_range(const string& t_ports)
{
    int min_port{0};
    int max_port{0};

    bool valid{true};
    const string_array_t port_bounds{algo::split<2>(t_ports, "-")};

    if (is_value(t_ports) && (valid = algo::is_integral(port_bounds, true)))
    {
        min_port = std::stoi(port_bounds[0]);
        max_port = std::stoi(port_bounds[1]);
    }

    if (valid && min_port < max_port)
    {
        for (const int& port_num : algo::iota(min_port, max_port))
        {
            // Allow (skip) port '0' when used in range
            if (port_num == PORT_NULL)
            {
                continue;
            }

            if (!net::valid_port(port_num))
            {
                valid = errorf("'%' is not a valid port number", port_num);
                break;
            }
            args.ports.emplace_back(static_cast<port_t>(port_num));
        }
    }
    else  // Invalid port range
    {
        valid = errorf("'%' is not a valid port range", t_ports);
    }

    return valid;
}

/**
* @brief
*     Parse and validate the given ports or port range and
*     update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_ports(const string& t_ports)
{
    bool valid{true};

    for (const string& port : algo::split(t_ports, ","))
    {
        if (is_port_range(port))
        {
            if (!(valid = parse_port_range(port)))
            {
                break;
            }
            continue;
        }

        if (!net::valid_port(port))
        {
            valid = errorf("'%' is not a valid port number", port);
            break;
        }
        args.ports.emplace_back(static_cast<port_t>(std::stoi(port)));
    }
    return valid;
}

/**
* @brief
*     Parse and validate the given ports or port range
*     and update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_ports(const IndexedArg& t_indexed_arg,
                                  List<size_t>& t_proc_indexes)
{
    if (!m_argv.valid_index(t_indexed_arg.index))
    {
        throw ArgEx{INVALID_ARG_INDEX_MSG, "t_indexed_arg"};
    }

    bool valid;
    const size_t value_index{t_indexed_arg.index + 1_sz};

    if (m_argv.valid_index(value_index) && is_value(m_argv[value_index]))
    {
        if (valid = parse_ports(m_argv[value_index]))
        {
            t_proc_indexes.emplace_back(value_index);
        }
    }
    else  // Missing value argument
    {
        valid = error("-p/--port PORT", ArgType::flag);
    }

    return valid;
}

/**
* @brief
*     Parse and validate the given thread count and
*     update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_threads(const IndexedArg& t_indexed_arg,
                                    List<size_t>& t_proc_indexes)
{
    if (!m_argv.valid_index(t_indexed_arg.index))
    {
        throw ArgEx{INVALID_ARG_INDEX_MSG, "t_indexed_arg"};
    }

    bool valid{true};
    const size_t value_index{t_indexed_arg.index + 1_sz};

    if (m_argv.valid_index(value_index) && is_value(m_argv[value_index]))
    {
        const string threads_str{m_argv[value_index]};

        const bool threads_str_integral{algo::is_integral(threads_str)};
        const size_t threads{threads_str_integral ? algo::to_uint(threads_str) : 0_sz};

        if (threads > 0)
        {
            args.threads = threads;
            t_proc_indexes.emplace_back(value_index);
        }
        else  // Invalid thread count
        {
            valid = errorf("'%' is not a valid thread pool size", threads_str);
        }
    }
    else  // Missing value argument
    {
        valid = error("-T/--threads NUM", ArgType::flag);
    }
    return valid;
}

/**
* @brief
*     Parse and validate the given socket timeout (in milliseconds)
*     and update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_timeout(const IndexedArg& t_indexed_arg,
                                    List<size_t>& t_proc_indexes)
{
    if (!m_argv.valid_index(t_indexed_arg.index))
    {
        throw ArgEx{INVALID_ARG_INDEX_MSG, "t_indexed_arg"};
    }

    bool valid{true};
    const size_t value_index{t_indexed_arg.index + 1_sz};

    if (m_argv.valid_index(value_index) && is_value(m_argv[value_index]))
    {
        const string ms{m_argv[value_index]};

        if (algo::is_integral(ms, true))
        {
            args.timeout = algo::to_uint(ms);
            t_proc_indexes.emplace_back(value_index);
        }
        else  // Invalid connection timeout
        {
            valid = errorf("'%' is not a valid connection timeout", ms);
        }
    }
    else  // Missing value argument
    {
        valid = error("-t/--timeout MS", ArgType::flag);
    }
    return valid;
}

/**
* @brief
*     Validate all arguments from the given command-line argument list.
*/
bool scan::ArgParser::validate(List<string>& t_list)
{
    m_valid = parse_aliases(t_list) && parse_flags(t_list);

    if (m_valid)
    {
        switch (t_list.size())
        {
            case 0:   // Missing TARGET
            {
                m_valid = error("TARGET", ArgType::value);
                break;
            }
            case 1:   // Syntax: TARGET
            {
                if (args.ports.empty())
                {
                    m_valid = error("PORT", ArgType::value);
                    break;
                }
                args.target = t_list[0];
                break;
            }
            case 2:   // Syntax: TARGET PORTS
            {
                if (!parse_ports(t_list[1]))
                {
                    m_valid = false;
                    break;
                }
                args.target = t_list[0];
                break;
            }
            default:  // Unrecognized argument
            {
                m_valid = errorf("Failed to validate: '%'", t_list.join("', '"));
                break;
            }
        }

        // Validate the target hostname/address
        if (m_valid && !args.target.valid())
        {
            if (net::valid_ipv4_fmt(args.target))
            {
                errorf("Invalid IPv4 address received: '%'", args.target);
            }
            else
            {
                error(asio::error::host_not_found);
            }
        }
    }
    return m_valid;
}

/**
* @brief
*     Write the application usage information and the given
*     network socket error to the standard error stream.
*/
std::string scan::ArgParser::error(const net_error_code_t& t_ecode)
{
    m_valid = false;
    std::cout << usage() << LF;

    const string error_msg{net::error(args.target.name(), t_ecode)};
    std::cout << LF;

    return error_msg;
}
