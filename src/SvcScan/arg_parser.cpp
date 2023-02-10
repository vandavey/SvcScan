/*
*  arg_parser.cpp
*  --------------
*  Source file for a command-line argument parser and validator
*/
#include <algorithm>
#include <cmath>
#include "includes/errors/null_ptr_ex.h"
#include "includes/inet/http/request.h"
#include "includes/io/filesys/path.h"
#include "includes/utils/arg_parser.h"

/**
* @brief  Command-line argument enumeration type.
*/
enum class scan::ArgParser::ArgType : uint8_t
{
    unknown,  // Unknown argument type
    alias,    // Argument alias (e.g., -f)
    flag,     // Argument flag (e.g., --foo)
    value     // Positional or named argument value
};

/**
* @brief  Initialize the object.
*/
scan::ArgParser::ArgParser()
{
    m_help_shown = m_valid = false;
    m_usage = algo::fstr("Usage: % [OPTIONS] TARGET", &EXE[0]);
}

/**
* @brief  Initialize the object.
*/
scan::ArgParser::ArgParser(const ArgParser &t_parser) noexcept
{
    m_argv = t_parser.m_argv;
    m_usage = t_parser.m_usage;
}

/**
* @brief  Get the application name and repository formatted as a title.
*/
std::string scan::ArgParser::app_title(const string &t_name_sep)
{
    const string delim{ t_name_sep.empty() ? " " : algo::fstr(" % ", t_name_sep) };
    return algo::fstr("%%(%)", &APP[0], delim, &REPO[0]);
}

/**
* @brief  Write the extended application usage information to the standard output
*         stream. Returns false to indicate that argument parsing succeeded.
*/
bool scan::ArgParser::help()
{
    m_help_shown = true;

    const List<string> usage_lines
    {
        app_title(),
        algo::concat(m_usage, &LF[0]),
        "Network service scanner application\n",
        "Positional Arguments:",
        "  TARGET                     Target IPv4 address or hostname\n",
        "Named Arguments:",
        "  -h/-?,    --help           Show this help message and exit",
        "  -v,       --verbose        Enable verbose console output",
        "  -s,       --ssl            Enable SSL/TLS socket connections",
        "  -j,       --json           Output scan results in JSON format",
        "  -p PORT,  --port PORT      Port number(s) - comma separated (no spaces)",
        "  -t MS,    --timeout MS     Connection timeout (milliseconds)",
        "                             [ Default: 3500 ]",
        "  -T NUM,   --threads NUM    Thread pool size (execution thread count)",
        "                             [ Default: local thread count ]",
        "  -o PATH,  --output PATH    Write the scan results to a file",
        "  -c URI,   --curl URI       Send an HTTP request to the specified URI\n",
        "Usage Examples:",
        "  svcscan.exe -v localhost 21,443,80",
        "  svcscan.exe -p 22-25,53 192.168.1.1",
        "  svcscan.exe -vt 500 192.168.1.1 4444",
        "  svcscan.exe --curl /admin 192.168.1.1 80",
    };

    std::cout << algo::concat(usage_lines.join_lines(), &LF[0], &LF[0]);
    return false;
}

/**
* @brief  Determine whether the extended application usage information was shown.
*/
bool scan::ArgParser::help_shown() const noexcept
{
    return m_help_shown;
}

/**
* @brief  Parse and validate the raw command-line arguments.
*/
bool scan::ArgParser::parse_argv(const int &t_argc, char *t_argv[])
{
    if (t_argv == nullptr)
    {
        throw NullPtrEx{ "t_argv" };
    }
    bool show_help{ false };

    args.exe_path = t_argv[0];
    m_argv = args.argv = defrag_argv(t_argc, t_argv);

    // Display program usage and stop validation
    if (t_argc == 1 || m_argv.any("-?", "-h", "--help"))
    {
        show_help = true;
    }

    return show_help ? help() : validate(m_argv);
}

/**
* @brief  Determine whether the given command-line argument is an argument alias.
*/
bool scan::ArgParser::is_alias(const string &t_arg)
{
    return std::regex_match(t_arg, regex(&ALIAS_REGEX[0]));
}

/**
* @brief  Determine whether the given command-line argument is an argument flag.
*/
bool scan::ArgParser::is_flag(const string &t_arg)
{
    return std::regex_match(t_arg, regex(&FLAG_REGEX[0]));
}

/**
* @brief  Determine whether the given port is in range notation (e.g., n1-n2).
*/
bool scan::ArgParser::is_port_range(const string &t_port)
{
    const bool valid_size{ t_port.find("-") != string::npos && t_port.size() > 2 };
    const bool valid_fmt{ t_port[0] != '-' && t_port[t_port.size()] != '-' };

    return valid_size && valid_fmt;
}

/**
* @brief  Determine whether the given command-line argument is a positional argument
*         or the value argument for an argument flag or an argument alias.
*/
bool scan::ArgParser::is_value(const string &t_arg)
{
    return !t_arg.empty() && !is_alias(t_arg) && !is_flag(t_arg);
}

/**
* @brief  Defragment the given command-line arguments so quoted
*         string arguments are properly parsed and validated.
*/
scan::List<std::string> scan::ArgParser::defrag_argv(const int &t_argc,
                                                     char *t_argv[]) {
    if (t_argc < 1)
    {
        throw ArgEx{ "t_argc", "Invalid argument count received" };
    }

    if (t_argv == nullptr)
    {
        throw NullPtrEx{ "t_argv" };
    }

    List<string> defrag_list;
    const List<string> frag_list{ algo::arg_vector(t_argc, t_argv) };

    // Defragment the given arguments
    for (size_t i{ 0 }; i < frag_list.size(); i++)
    {
        const bool beg_quoted{ frag_list[i].starts_with('\'') };

        if (!beg_quoted || (beg_quoted && frag_list[i].ends_with('\'')))
        {
            defrag_list.add(frag_list[i]);
            continue;
        }

        if (i == frag_list.size() - 1)
        {
            defrag_list.add(frag_list[i]);
            break;
        }

        // Locate terminating argument and parse the range
        for (size_t j{ i + 1 }; j < frag_list.size(); j++)
        {
            if (frag_list[j].ends_with('\''))
            {
                defrag_list.add(frag_list.slice(i, j + 1).join(" "));
                i = j;
                break;
            }
        }
    }

    return defrag_list;
}

/**
* @brief  Write the application usage information and a command-line argument
*         error message to the standard error stream.
*/
bool scan::ArgParser::error(const string &t_arg,
                            const ArgType &t_type,
                            const bool &t_valid) {
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
* @brief  Parse and validate the given command-line argument flag aliases
*         and their corresponding arguments (e.g., -f foo).
*/
bool scan::ArgParser::parse_aliases(List<string> &t_list)
{
    if (t_list.contains("-"))
    {
        return error("-", ArgType::unknown);
    }
    bool valid{ true };

    // TODO: Simplify/cleanup this garbage

    // Validate arg aliases and values
    for (const string &arg : t_list.copy())
    {
        // Skip non-alias arguments
        if (!is_alias(arg))
        {
            continue;
        }

        for (const char &ch : arg)
        {
            switch (ch)
            {
                case '-':  // Ignore character
                {
                    break;
                }
                case '?':  // Show usage information
                case 'h':  // Show usage information
                {
                    valid = help();
                    break;
                }
                case 'v':  // Enable verbose output
                {
                    args.verbose = true;
                    break;
                }
                case 's':  // Enable SSL/TLS connections
                {
                    args.tls_enabled = true;
                    break;
                }
                case 'j':  // Output scan results in JSON format
                {
                    args.out_json = true;
                    break;
                }
                case 'p':  // Parse and validate target port(s)
                {
                    if (arg == t_list.last() || !arg.ends_with('p'))
                    {
                        valid = error("-p PORT", ArgType::alias);
                        break;
                    }
                    valid = set_ports(t_list[t_list.find(arg) + 1]);
                    break;
                }
                case 't':  // Parse and validate connection timeout
                {
                    if (arg == t_list.last() || !arg.ends_with('t'))
                    {
                        valid = error("-t MS", ArgType::alias);
                        break;
                    }
                    valid = set_timeout(t_list[t_list.find(arg) + 1]);
                    break;
                }
                case 'T':  // Parse and validate thread count
                {
                    if (arg == t_list.last() || !arg.ends_with('T'))
                    {
                        valid = error("-T NUM", ArgType::alias);
                        break;
                    }
                    valid = set_threads(t_list[t_list.find(arg) + 1]);
                    break;
                }
                case 'o':  // Parse and validate output file path
                {
                    if (arg == t_list.last() || !arg.ends_with('o'))
                    {
                        valid = error("-o PATH", ArgType::alias);
                        break;
                    }
                    valid = set_path(t_list[t_list.find(arg) + 1]);
                    break;
                }
                case 'c':  // Parse and validate HTTP request URI
                {
                    string uri{ &URI_ROOT[0] };
                    bool remove_arg{ arg != t_list.last() && arg.ends_with('c') };

                    if (remove_arg)
                    {
                        const string next_arg{ t_list[t_list.find(arg) + 1] };
                        uri = (remove_arg = is_value(next_arg)) ? next_arg : uri;
                    }
                    valid = set_curl_uri(uri, remove_arg);
                    break;
                }
                default:   // Unrecognized alias name
                {
                    valid = errorf("Unrecognized flag: '%'", arg);
                    break;
                }
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
        t_list.remove(arg);
    }

    return valid;
}

/**
* @brief  Parse and validate the given command-line argument flags
*         and their corresponding arguments (e.g., --flag foo).
*/
bool scan::ArgParser::parse_flags(List<string> &t_list)
{
    if (t_list.contains("--"))
    {
        return error("--", ArgType::unknown);
    }
    bool valid{ true };

    // TODO: Simplify/cleanup this garbage

    for (const string &arg : t_list.copy())
    {
        // Skip non-flag arguments
        if (!is_flag(arg))
        {
            continue;
        }

        // Show usage information
        if (arg == "--help")
        {
            valid = help();
            break;
        }

        // Enable verbose console output
        if (arg == "--verbose")
        {
            args.verbose = true;
            t_list.remove(arg);
            continue;
        }

        // Output scan results in JSON format
        if (arg == "--json")
        {
            args.out_json = true;
            t_list.remove(arg);
            continue;
        }

        // Enable SSL/TLS connections
        if (arg == "--ssl")
        {
            args.tls_enabled = true;
            t_list.remove(arg);
            continue;
        }

        // Parse and validate target port(s)
        if (arg == "--port")
        {
            if (arg == t_list.last())
            {
                valid = error("--port PORT", ArgType::flag);
                break;
            }
            else if (!(valid = set_ports(t_list[t_list.find(arg) + 1])))
            {
                break;
            }
            t_list.remove(arg);
            continue;
        }

        // Parse and validate connection timeout
        if (arg == "--timeout")
        {
            if (arg == t_list.last())
            {
                valid = error("--timeout MS", ArgType::flag);
                break;
            }
            else if (!(valid = set_timeout(t_list[t_list.find(arg) + 1])))
            {
                break;
            }
            t_list.remove(arg);
            continue;
        }

        // Parse and validate thread count
        if (arg == "--threads")
        {
            if (arg == t_list.last())
            {
                valid = error("--threads NUM", ArgType::flag);
                break;
            }
            else if (!(valid = set_threads(t_list[t_list.find(arg) + 1])))
            {
                break;
            }
            t_list.remove(arg);
            continue;
        }

        // Parse and validate output file path
        if (arg == "--output")
        {
            if (arg == t_list.last())
            {
                valid = error("--output PATH", ArgType::flag);
                break;
            }
            else if (!(valid = set_path(t_list[t_list.find(arg) + 1])))
            {
                break;
            }
            t_list.remove(arg);
            continue;
        }

        // Parse and validate HTTP request URI
        if (arg == "--curl")
        {
            string uri{ &URI_ROOT[0] };
            bool remove_arg{ arg != t_list.last() };

            if (remove_arg)
            {
                const string next_arg{ t_list[t_list.find(arg) + 1] };
                uri = (remove_arg = is_value(next_arg)) ? next_arg : uri;
            }

            if (!(valid = set_curl_uri(uri, remove_arg)))
            {
                break;
            }
            t_list.remove(arg);
            continue;
        }

        // Unrecognized flag name
        if (!valid)
        {
            valid = errorf("Unrecognized flag: '%'", arg);
            break;
        }
    }

    return valid;
}

/**
* @brief  Parse and validate the given HTTP request URI and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_curl_uri(const string &t_uri, const bool &t_remove_arg)
{
    bool valid{ true };

    if (is_value(t_uri) && Request<>::valid_uri(t_uri))
    {
        args.curl = true;
        args.uri = t_uri;

        if (t_remove_arg)
        {
            m_argv.remove(t_uri);
        }
    }
    else  // Invalid URI was received
    {
        valid = errorf("'%' is not a valid HTTP URI", t_uri);
    }
    return valid;
}

/**
* @brief  Parse and validate the given report output path and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_path(const string &t_path)
{
    bool valid{ true };

    // Invalid file path argument
    if (!is_value(t_path) || !Path::valid_file(t_path))
    {
        switch (Path::path_info(t_path))
        {
            case PathInfo::empty:
                valid = errorf("File path cannot lead to a directory: '%'", t_path);
                break;
            case PathInfo::directory:
                valid = errorf("File path cannot lead to a directory: '%'", t_path);
                break;
            case PathInfo::parent_not_found:
                valid = errorf("Nonexistent parent path: '%'", Path::parent(t_path));
                break;
            default:
                valid = errorf("Invalid output file path: '%'", t_path);
                break;
        }
    }
    else  // Valid output path
    {
        args.out_path = Path::resolve(t_path);
        m_argv.remove(t_path);
    }
    return valid;
}

/**
* @brief  Parse and validate the given port range and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_port_range(const string &t_ports)
{
    int min_port{ 0 };
    int max_port{ 0 };

    bool valid{ true };
    const string_array<2> port_bounds{ algo::split<2>(t_ports, "-") };

    if (is_value(t_ports) && (valid = algo::is_integral(port_bounds, true)))
    {
        min_port = std::stoi(port_bounds[0]);
        max_port = std::stoi(port_bounds[1]);
    }

    if (valid && min_port < max_port)
    {
        for (const int &port_num : List<int>::fill(min_port, max_port))
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
            args.ports.add(static_cast<port_t>(port_num));
        }
    }
    else  // Invalid port range
    {
        valid = errorf("'%' is not a valid port range", t_ports);
    }

    return valid;
}

/**
* @brief  Parse and validate the given ports or port range and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_ports(const string &t_ports)
{
    bool valid{ true };

    if (is_value(t_ports))
    {
        for (const string &port : algo::split(t_ports, ","))
        {
            if (is_port_range(port))
            {
                if (!(valid = set_port_range(port)))
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
            args.ports.add(algo::to_word(port));
        }
    }

    if (valid)
    {
        m_argv.remove(t_ports);
    }
    return valid;
}

/**
* @brief  Parse and validate the given thread count and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_threads(const string &t_threads)
{
    bool valid{ is_value(t_threads) && algo::is_integral(t_threads, true) };

    if (valid && algo::to_uint(t_threads) >= 1)
    {
        args.threads = algo::to_uint(t_threads);
        m_argv.remove(t_threads);
    }
    else  // Invalid thread count
    {
        valid = errorf("'%' not a valid thread pool thread count", t_threads);
    }
    return valid;
}

/**
* @brief  Parse and validate the given socket timeout (in milliseconds)
*         and update the underlying command-line arguments.
*/
bool scan::ArgParser::set_timeout(const string &t_ms)
{
    bool valid{ true };

    // Update the connection timeout
    if (is_value(t_ms) && algo::is_integral(t_ms, true))
    {
        args.timeout = algo::to_uint(t_ms);
        m_argv.remove(t_ms);
    }
    else  // Expected integral value
    {
        valid = errorf("'%' is not a valid connection timeout", t_ms);
    }
    return valid;
}

/**
* @brief  Validate all arguments from the given command-line argument list.
*/
bool scan::ArgParser::validate(List<string> &t_list)
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
                if (!set_ports(t_list[1]))
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
        if (m_valid && !args.target.is_valid())
        {
            if (net::valid_ipv4_fmt(args.target))
            {
                errorf("Invalid IPv4 address received: '%'", args.target);
            }
            else
            {
                error(error::host_not_found);
            }
        }
    }
    return m_valid;
}

/**
* @brief  Write the application usage information and the given network
*         socket error to the standard error stream.
*/
std::string scan::ArgParser::error(const error_code &t_ecode)
{
    m_valid = false;
    std::cout << m_usage << &LF[0];

    const string error_msg{ net::error(args.target.name(), t_ecode) };
    std::cout << &LF[0];

    return error_msg;
}
