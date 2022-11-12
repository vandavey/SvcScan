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
    unknown,  // Unknown argument
    flag,     // Syntax: -f, --foo
    value     // Syntax: --foobar <value>
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
*         stream. Always returns false to indicate that no error occurred.
*/
bool scan::ArgParser::help()
{
    m_help_shown = true;

    const List<string> usage_lines
    {
        app_title(),
        m_usage + &LF[0],
        "Network service scanner application\n",
        "Positional Arguments:",
        "  TARGET                     Target IPv4 address or hostname\n",
        "Named Arguments:",
        "  -v,       --verbose        Enable verbose console output",
        "  -s,       --ssl            Enable SSL/TLS socket connections",
        "  -j,       --json           Output scan results in JSON format",
        "  -p PORT,  --port PORT      Port(s) - comma separated (no spaces)",
        "  -t MS,    --timeout MS     Connection timeout (milliseconds)",
        "                             [ Default: 3500 ]",
        "  -T NUM,   --threads NUM    Thread pool size (execution thread count)",
        "                             [ Default: local thread count ]",
        "  -o PATH,  --output PATH    Write scan output to text file",
        "  -u URI,   --uri URI        URI to use when sending HTTP requests",
        "                             [ Default: '/' ]",
        "  -h/-?,    --help           Show this help message and exit\n",
        "Usage Examples:",
        "  svcscan.exe -v localhost 21,443,80",
        "  svcscan.exe -p 22-25,53 192.168.1.1",
        "  svcscan.exe -vt 500 192.168.1.1 4444",
        "  svcscan.exe -p 80 192.168.1.1 --uri /admin",
    };

    std::cout << usage_lines.join_lines() << &LF[0] << &LF[0];
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

    if (t_argc <= 0)
    {
        throw ArgEx{ "t_argc", "Invalid argument count received" };
    }
    bool show_help{ false };

    // Parse arguments (exclude program path)
    for (int i{ 1 }; i < t_argc; i++)
    {
        if (t_argv[i] != nullptr)
        {
            m_argv.add(t_argv[i]);
        }
    }

    // Display program usage and stop validation
    if (t_argc == 1 || m_argv.any("-?", "-h", "--help"))
    {
        show_help = true;
    }

    return show_help ? help() : validate(m_argv);
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
        case ArgType::flag:
            valid = errorf("Missing flag argument: '%'", t_arg, t_valid);
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
* @brief  Parse and validate the given command-line flag abbreviation
*         arguments and their corresponding values (e.g., -f foo).
*/
bool scan::ArgParser::parse_aliases(List<string> &t_list)
{
    if (t_list.contains("-"))
    {
        return error("-", ArgType::unknown);
    }
    bool valid{ true };

    // Validate arg aliases and values
    for (const string &elem : t_list.copy())
    {
        // Skip non-alias arguments
        if (elem.size() < 2 || elem[0] != '-' || elem[1] == '-')
        {
            continue;
        }

        for (const char &ch : elem)
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
                    if (elem == t_list.last())
                    {
                        valid = error("-p PORT", ArgType::flag);
                        break;
                    }
                    valid = set_ports(t_list[t_list.find(elem, 0, 1)]);
                    break;
                }
                case 't':  // Parse and validate connection timeout
                {
                    if (elem == t_list.last())
                    {
                        valid = error("-t MS", ArgType::flag);
                        break;
                    }
                    valid = set_timeout(t_list[t_list.find(elem, 0, 1)]);
                    break;
                }
                case 'T':  // Parse and validate thread count
                {
                    if (elem == t_list.last())
                    {
                        valid = error("-T NUM", ArgType::flag);
                        break;
                    }
                    valid = set_threads(t_list[t_list.find(elem, 0, 1)]);
                    break;
                }
                case 'o':  // Parse and validate output file path
                {
                    if (elem == t_list.last())
                    {
                        valid = error("-o PATH", ArgType::flag);
                        break;
                    }
                    valid = set_path(t_list[t_list.find(elem, 0, 1)]);
                    break;
                }
                case 'u':  // Parse and validate HTTP request URI
                {
                    if (elem == t_list.last())
                    {
                        valid = error("--uri URI", ArgType::flag);
                        break;
                    }
                    valid = set_uri(t_list[t_list.find(elem, 0, 1)]);
                    break;
                }
                default:   // Unrecognized alias name
                {
                    valid = errorf("Unrecognized flag: '%'", elem);
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
        t_list.remove(elem);
    }

    return valid;
}

/**
* @brief  Parse and validate the given command-line flag arguments
*         and their corresponding values (e.g., --flag foo).
*/
bool scan::ArgParser::parse_flags(List<string> &t_list)
{
    if (t_list.contains("--"))
    {
        return error("--", ArgType::unknown);
    }
    bool valid{ true };

    for (const string &elem : t_list.copy())
    {
        // Skip non-flag arguments
        if (elem.size() < 3 || elem.rfind("--") != 0)
        {
            continue;
        }

        // Show usage information
        if (elem == "--help")
        {
            valid = help();
            break;
        }

        // Enable verbose console output
        if (elem == "--verbose")
        {
            args.verbose = true;
            t_list.remove(elem);
            continue;
        }

        // Output scan results in JSON format
        if (elem == "--json")
        {
            args.out_json = true;
            t_list.remove(elem);
            continue;
        }

        // Enable SSL/TLS connections
        if (elem == "--ssl")
        {
            args.tls_enabled = true;
            t_list.remove(elem);
            continue;
        }

        // Parse and validate target port(s)
        if (elem == "--port")
        {
            if (elem == t_list.last())
            {
                valid = error("--port PORT", ArgType::flag);
                break;
            }
            else if (!(valid = set_ports(t_list[t_list.find(elem, 0, 1)])))
            {
                break;
            }
            t_list.remove(elem);
            continue;
        }

        // Parse and validate connection timeout
        if (elem == "--timeout")
        {
            if (elem == t_list.last())
            {
                valid = error("--timeout MS", ArgType::flag);
                break;
            }
            else if (!(valid = set_timeout(t_list[t_list.find(elem, 0, 1)])))
            {
                break;
            }
            t_list.remove(elem);
            continue;
        }

        // Parse and validate thread count
        if (elem == "--threads")
        {
            if (elem == t_list.last())
            {
                valid = error("--threads NUM", ArgType::flag);
                break;
            }
            else if (!(valid = set_threads(t_list[t_list.find(elem, 0, 1)])))
            {
                break;
            }
            t_list.remove(elem);
            continue;
        }

        // Parse and validate output file path
        if (elem == "--output")
        {
            if (elem == t_list.last())
            {
                valid = error("--output PATH", ArgType::flag);
                break;
            }
            else if (!(valid = set_path(t_list[t_list.find(elem, 0, 1)])))
            {
                break;
            }
            t_list.remove(elem);
            continue;
        }

        // Parse and validate HTTP request URI
        if (elem == "--uri")
        {
            if (elem == t_list.last())
            {
                valid = error("--uri URI", ArgType::flag);
                break;
            }
            else if (!(valid = set_uri(t_list[t_list.find(elem, 0, 1)])))
            {
                break;
            }
            t_list.remove(elem);
            continue;
        }

        // Unrecognized flag name
        if (!valid)
        {
            valid = errorf("Unrecognized flag: '%'", elem);
            break;
        }
    }

    return valid;
}

/**
* @brief  Parse and validate the given report output path and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_path(const string &t_path)
{
    const bool valid_path{ Path::valid_file(t_path) };

    // Invalid file path argument
    if (!valid_path)
    {
        switch (Path::path_info(t_path))
        {
            case PathInfo::empty:
                errorf("File path cannot lead to a directory: '%'", t_path);
                break;
            case PathInfo::directory:
                errorf("File path cannot lead to a directory: '%'", t_path);
                break;
            case PathInfo::parent_not_found:
                errorf("Unable to locate parent path: '%'", Path::parent(t_path));
                break;
            default:
                errorf("Invalid output file path: '%'", t_path);
                break;
        }
    }
    else  // Valid output path
    {
        args.out_path = Path::resolve(t_path);
        m_argv.remove(t_path);
    }
    return valid_path;
}

/**
* @brief  Parse and validate the given port range and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_port_range(const string &t_ports)
{
    int min_port{ 0 };
    int max_port{ 0 };

    bool valid_ports{ false };
    const string_array<2> port_bounds{ algo::split_n<2>(t_ports, "-") };

    if (!t_ports.empty() && algo::is_integral(port_bounds))
    {
        min_port = std::stoi(port_bounds[0]);
        max_port = std::stoi(port_bounds[1]);
    }

    if (min_port < max_port)
    {
        for (const int &port_num : List<int>::fill(min_port, max_port))
        {
            // Allow (skip) port '0' when used in range
            if (port_num == 0)
            {
                continue;
            }

            if (!net::valid_port(port_num))
            {
                valid_ports = errorf("'%' is not a valid port number", port_num);
                break;
            }
            args.ports.add(port_num);
        }
    }
    else  // Invalid port range
    {
        valid_ports = errorf("'%' is not a valid port range", t_ports);
    }

    return valid_ports;
}

/**
* @brief  Parse and validate the given ports or port range and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_ports(const string &t_ports)
{
    bool valid_ports{ !t_ports.empty() };

    for (const string &port : algo::split(t_ports, ","))
    {
        if (is_port_range(port))
        {
            if (!(valid_ports = set_port_range(port)))
            {
                break;
            }
            continue;
        }

        if (!net::valid_port(port))
        {
            valid_ports = errorf("'%' is not a valid port number", port);
            break;
        }
        args.ports.add(std::stoi(port));
    }

    if (valid_ports)
    {
        m_argv.remove(t_ports);
    }
    return valid_ports;
}

/**
* @brief  Parse and validate the given thread count and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_threads(const string &t_threads)
{
    bool valid_threads{ algo::is_integral(t_threads) && std::stoi(t_threads) > 0 };

    if (valid_threads)
    {
        args.threads = static_cast<uint_t>(std::stoi(t_threads));
        m_argv.remove(t_threads);
    }
    else  // Invalid thread count
    {
        errorf("'%' not a valid thread pool thread count", t_threads);
    }
    return valid_threads;
}

/**
* @brief  Parse and validate the given socket timeout (in milliseconds)
*         and update the underlying command-line arguments.
*/
bool scan::ArgParser::set_timeout(const string &t_ms)
{
    bool valid_timeout{ true };

    // Update the connection timeout
    if (algo::is_integral(t_ms))
    {
        args.timeout = std::stoi(t_ms);
        m_argv.remove(t_ms);
    }
    else  // Expected integral value
    {
        valid_timeout = errorf("'%' is not a valid connection timeout", t_ms);
    }
    return valid_timeout;
}

/**
* @brief  Parse and validate the given HTTP request URI and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_uri(const string &t_uri)
{
    string uri_str{ t_uri };

    // Ensure URI begins with '/'
    if (!uri_str.empty())
    {
        uri_str = t_uri.starts_with('/') ? t_uri : algo::fstr("/%", t_uri);
    }
    bool valid_uri;

    // Validate the URI using regex
    if (valid_uri = Request<>::valid_uri(t_uri))
    {
        args.uri = uri_str;
        m_argv.remove(t_uri);
    }
    else  // Invalid URI received
    {
        valid_uri = errorf("'%' is not a valid HTTP URI", t_uri);
    }
    return valid_uri;
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
                m_valid = errorf("Failed to validate: '%'", t_list.join(", "));
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
