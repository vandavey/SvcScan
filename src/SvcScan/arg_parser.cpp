/*
* @file
*     arg_parser.cpp
* @brief
*     Source file for a command-line argument parser and validator.
*/
#include <algorithm>
#include <cmath>
#include "includes/errors/null_ptr_ex.h"
#include "includes/inet/http/request.h"
#include "includes/io/filesys/path.h"
#include "includes/utils/arg_parser.h"

/**
* @brief
*     Command-line argument enumeration type.
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
*     Initialize the object.
*/
scan::ArgParser::ArgParser()
{
    m_help_shown = m_valid = false;
    m_usage = algo::fstr("Usage: % [OPTIONS] TARGET", EXE);
}

/**
* @brief
*     Initialize the object.
*/
scan::ArgParser::ArgParser(const ArgParser &t_parser) noexcept
{
    m_argv = t_parser.m_argv;
    m_help_shown = t_parser.m_help_shown;
    m_usage = t_parser.m_usage;
    m_valid = t_parser.m_valid;
}

/**
* @brief
*     Get the application name and repository formatted as a title.
*/
std::string scan::ArgParser::app_title(const string &t_name_sep)
{
    const string delim{ t_name_sep.empty() ? " " : algo::fstr(" % ", t_name_sep) };
    return algo::fstr("%%(%)", APP, delim, REPO);
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
        app_title(),
        algo::concat(m_usage, LF),
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

    std::cout << algo::concat(usage_lines.join_lines(), LF, LF);
    return false;
}

/**
* @brief
*     Determine whether the extended application usage information was shown.
*/
bool scan::ArgParser::help_shown() const noexcept
{
    return m_help_shown;
}

/**
* @brief
*     Parse and validate the raw command-line arguments.
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
* @brief
*     Determine whether the given command-line argument is an argument alias.
*/
bool scan::ArgParser::is_alias(const string &t_arg)
{
    return algo::matches(t_arg, ALIAS_RGX);
}

/**
* @brief
*     Determine whether the given command-line argument is an argument flag.
*/
bool scan::ArgParser::is_flag(const string &t_arg)
{
    return algo::matches(t_arg, FLAG_RGX);
}

/**
* @brief
*     Determine whether the given port is in range notation (e.g., n1-n2).
*     Do not yet check whether range bounds are integral.
*/
bool scan::ArgParser::is_port_range(const string &t_port)
{
    const bool valid_size{ t_port.find("-") != string::npos && t_port.size() > 2 };
    const bool valid_fmt{ t_port[0] != '-' && t_port[t_port.size()] != '-' };

    return valid_size && valid_fmt;
}

/**
* @brief
*     Determine whether the given command-line argument is a positional argument
*     or the value argument for an argument flag or an argument alias.
*/
bool scan::ArgParser::is_value(const string &t_arg)
{
    return algo::matches(t_arg, POS_RGX);
}

/**
* @brief
*     Defragment the given command-line arguments so quoted
*     string arguments are properly parsed and validated.
*/
scan::List<std::string> scan::ArgParser::defrag_argv(const int &t_argc, char *t_argv[])
{
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
* @brief
*     Remove processed command-line arguments according to the given indexes.
*/
void scan::ArgParser::remove_processed_args(const vector<size_t> &t_indexes)
{
    size_t delta{ 0 };

    for (const size_t &index : algo::sort(t_indexes))
    {
        m_argv.remove_at(index - delta++);
    }
}

/**
* @brief
*     Write the application usage information and an
*     error message to the standard error stream.
*/
bool scan::ArgParser::error(const string &t_msg, const bool &t_valid)
{
    std::cout << m_usage << LF;
    stdu::error(t_msg);
    std::cout << LF;

    return m_valid = t_valid;
}

/**
* @brief
*     Write the application usage information and a command-line
*     argument error message to the standard error stream.
*/
bool scan::ArgParser::error(const string &t_arg,
                            const ArgType &t_type,
                            const bool &t_valid)
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
*     Parse and validate the given command-line argument flag
*     aliases and their corresponding arguments (e.g., -f foo).
*/
bool scan::ArgParser::parse_aliases(List<string> &t_list)
{
    bool valid{ true };
    List<size_t> proc_indexes;

    const idx_pairs_t<string> idx_pairs{ algo::enumerate(t_list, ALIAS_RGX) };

    for (const IndexPair<string> &alias_pair : idx_pairs)
    {
        for (const char &ch : alias_pair.value)
        {
            switch (ch)
            {
                case '-':
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
                    valid = parse_ports(alias_pair, proc_indexes);
                    break;
                case 't':
                    valid = parse_timeout(alias_pair, proc_indexes);
                    break;
                case 'T':
                    valid = parse_threads(alias_pair, proc_indexes);
                    break;
                case 'o':
                    valid = parse_path(alias_pair, proc_indexes);
                    break;
                case 'c':
                    valid = parse_curl_uri(alias_pair, proc_indexes);
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
        proc_indexes.add(alias_pair.index);
    }
    remove_processed_args(proc_indexes);

    return valid;
}

/**
* @brief
*     Parse and validate the HTTP request URI argument
*     the underlying command-line arguments list.
*/
bool scan::ArgParser::parse_curl_uri(const IndexPair<string> &t_pair,
                                     List<size_t> &t_proc_indexes)
{
    if (!m_argv.valid_index(t_pair.index))
    {
        throw ArgEx{ "t_pair", "Invalid argument index" };
    }
    args.curl = true;

    bool valid{ true };
    const size_t value_idx{ t_pair.index + 1 };

    if (m_argv.valid_index(value_idx) && is_value(m_argv[value_idx]))
    {
        const string uri{ m_argv[value_idx] };

        if (Request<>::valid_uri(uri))
        {
            args.uri = uri;
            t_proc_indexes.add(value_idx);
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
bool scan::ArgParser::parse_flags(List<string> &t_list)
{
    if (t_list.contains("--"))
    {
        return error("--", ArgType::unknown);
    }

    bool valid{ true };
    List<size_t> proc_indexes;

    const idx_pairs_t<string> idx_pairs{ algo::enumerate(t_list, FLAG_RGX) };

    for (const IndexPair<string> &flag_pair : idx_pairs)
    {
        if (flag_pair.value == "--help")
        {
            valid = help();
        }
        else if (flag_pair.value == "--verbose")
        {
            args.verbose = true;
        }
        else if (flag_pair.value == "--json")
        {
            args.out_json = true;
        }
        else if (flag_pair.value == "--ssl")
        {
            args.tls_enabled = true;
        }
        else if (flag_pair.value == "--port")
        {
            valid = parse_ports(flag_pair, proc_indexes);
        }
        else if (flag_pair.value == "--timeout")
        {
            valid = parse_timeout(flag_pair, proc_indexes);
        }
        else if (flag_pair.value == "--threads")
        {
            valid = parse_threads(flag_pair, proc_indexes);
        }
        else if (flag_pair.value == "--output")
        {
            valid = parse_path(flag_pair, proc_indexes);
        }
        else if (flag_pair.value == "--curl")
        {
            valid = parse_curl_uri(flag_pair, proc_indexes);
        }
        else  // Unknown flag name
        {
            valid = errorf("Unrecognized flag: '%'", flag_pair.value);
        }

        if (!valid)
        {
            break;
        }
        proc_indexes.add(flag_pair.index);
    }
    remove_processed_args(proc_indexes);

    return valid;
}

/**
* @brief
*     Parse and validate the given report output path and
*     update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_path(const IndexPair<string> &t_pair,
                                 List<size_t> &t_proc_indexes)
{
    if (!m_argv.valid_index(t_pair.index))
    {
        throw ArgEx{ "t_pair", "Invalid argument index" };
    }

    bool valid{ true };
    const size_t value_idx{ t_pair.index + 1 };

    if (m_argv.valid_index(value_idx) && is_value(m_argv[value_idx]))
    {
        const string path{ m_argv[value_idx] };

        if (!Path::valid_file(path))
        {
            switch (Path::path_info(path))
            {
                case PathInfo::empty:
                    valid = errorf("Output file path cannot be empty", path);
                    break;
                case PathInfo::directory:
                    valid = errorf("File path leads to a directory: '%'", path);
                    break;
                case PathInfo::parent_not_found:
                    valid = errorf("Parent path not found: '%'", Path::parent(path));
                    break;
                default:
                    valid = errorf("Invalid output file path: '%'", path);
                    break;
            }
        }
        else  // Valid output path
        {
            args.out_path = Path::resolve(path);
            t_proc_indexes.add(value_idx);
        }
    }
    else  // Missing value argument
    {
        valid = error("-o/--output PATH", ArgType::flag);
    }

    return valid;
}

/**
* @brief
*     Parse and validate the given port range and
*     update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_port_range(const string &t_ports)
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
* @brief
*     Parse and validate the given ports or port range and
*     update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_ports(const string &t_ports)
{
    bool valid{ true };

    for (const string &port : algo::split(t_ports, ","))
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
        args.ports.add(static_cast<port_t>(std::stoi(port)));
    }
    return valid;
}

/**
* @brief
*     Parse and validate the given ports or port range
*     and update the underlying command-line arguments.
*/
bool scan::ArgParser::parse_ports(const IndexPair<string> &t_pair,
                                  List<size_t> &t_proc_indexes)
{
    if (!m_argv.valid_index(t_pair.index))
    {
        throw ArgEx{ "t_pair", "Invalid argument index" };
    }

    bool valid;
    const size_t value_idx{ t_pair.index + 1 };

    if (m_argv.valid_index(value_idx) && is_value(m_argv[value_idx]))
    {
        if (valid = parse_ports(m_argv[value_idx]))
        {
            t_proc_indexes.add(value_idx);
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
bool scan::ArgParser::parse_threads(const IndexPair<string> &t_pair,
                                    List<size_t> &t_proc_indexes)
{
    if (!m_argv.valid_index(t_pair.index))
    {
        throw ArgEx{ "t_pair", "Invalid argument index" };
    }

    bool valid{ true };
    const size_t value_idx{ t_pair.index + 1 };

    if (m_argv.valid_index(value_idx) && is_value(m_argv[value_idx]))
    {
        const string threads{ m_argv[value_idx] };

        if (algo::is_integral(threads) && algo::to_uint(threads) >= 1)
        {
            args.threads = algo::to_uint(threads);
            t_proc_indexes.add(value_idx);
        }
        else  // Invalid thread count
        {
            valid = errorf("'%' not a valid thread pool thread count", threads);
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
bool scan::ArgParser::parse_timeout(const IndexPair<string> &t_pair,
                                    List<size_t> &t_proc_indexes)
{
    if (!m_argv.valid_index(t_pair.index))
    {
        throw ArgEx{ "t_pair", "Invalid argument index" };
    }

    bool valid{ true };
    const size_t value_idx{ t_pair.index + 1 };

    if (m_argv.valid_index(value_idx) && is_value(m_argv[value_idx]))
    {
        const string ms{ m_argv[value_idx] };

        if (algo::is_integral(ms, true))
        {
            args.timeout = algo::to_uint(ms);
            t_proc_indexes.add(value_idx);
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
* @brief
*     Write the application usage information and the given
*     network socket error to the standard error stream.
*/
std::string scan::ArgParser::error(const error_code &t_ecode)
{
    m_valid = false;
    std::cout << m_usage << LF;

    const string error_msg{ net::error(args.target.name(), t_ecode) };
    std::cout << LF;

    return error_msg;
}
