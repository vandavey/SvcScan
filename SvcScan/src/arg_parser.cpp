/*
*  arg_parser.cpp
*  --------------
*  Source file for a command-line argument parser and validator
*/
#include <algorithm>
#include <cmath>
#include "includes/except/null_ptr_ex.h"
#include "includes/inet/scanners/scanner.h"
#include "includes/utils/arg_parser.h"

/**
* @brief  Command-line argument enumeration type.
*/
enum class scan::ArgParser::ArgType : short
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
    m_usage = Util::fstr("Usage: % [OPTIONS] TARGET", EXE);
    valid = help_shown = false;
}

/**
* @brief  Initialize the object.
*/
scan::ArgParser::ArgParser(const ArgParser &t_parser)
{
    m_argv = t_parser.m_argv;
    m_usage = t_parser.m_usage;
}

/**
* @brief  Write the extended application usage information to the standard output
*         stream. Always returns false to indicate that no error occurred.
*/
bool scan::ArgParser::help()
{
    help_shown = true;

    const vector<string> usage_lines
    {
        Util::fstr("SvcScan (%)", REPO),
        m_usage + LF,
        "TCP socket application banner grabber\n",
        "Positional Arguments:",
        "  TARGET                    Target address or domain name\n",
        "Optional Arguments:",
        "  -h/-?,   --help           Show this help message and exit",
        "  -v,      --verbose        Enable verbose console output",
        "  -s,      --ssl            Enable SSL/TLS socket connections",
        "  -p PORT, --port PORT      Port(s) - comma separated (no spaces)",
        "  -t MS,   --timeout MS     Connection timeout (milliseconds)",
        "                            [Default: 3500]",
        "  -u URI,  --uri URI        URI to use when sending HTTP requests",
        "  -o PATH, --output PATH    Write scan output to text file\n",
        "Usage Examples:",
        "  svcscan.exe -v localhost 21,443,80",
        "  svcscan.exe -p 22-25,53 192.168.1.1",
        "  svcscan.exe -vt 500 192.168.1.1 4444",
        "  svcscan.exe -p 80 192.168.1.1 --uri /admin",
    };

    std::cout << List<string>(usage_lines).join_lines() << LF << LF;
    return false;
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
                            const ArgType &t_arg_type,
                            const bool &t_valid) {
    bool is_valid;

    switch (t_arg_type)
    {
        case ArgType::unknown:  // Unknown argument
            is_valid = errorf("Unable to validate argument: '%'", t_arg, t_valid);
            break;
        case ArgType::flag:     // Missing flag
            is_valid = errorf("Missing flag argument: '%'", t_arg, t_valid);
            break;
        case ArgType::value:    // Missing value
            is_valid = errorf("Missing required argument(s): '%'", t_arg, t_valid);
            break;
        default:
            is_valid = t_valid;
            break;
    }
    return is_valid;
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
                    return help();
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
                case 'o':  // Output file path
                {
                    if (elem == t_list[-1])
                    {
                        return error("-o PATH", ArgType::flag);
                    }

                    // Parse/validate output path
                    if (!set_path(t_list[t_list.find(elem, 0, 1)]))
                    {
                        return false;
                    }
                    break;
                }
                case 't':  // Socket timeout
                {
                    if (elem == t_list[-1])
                    {
                        return error("-t MS", ArgType::flag);
                    }

                    // Parse/validate connection timeout
                    if (!set_timeout(t_list[t_list.find(elem, 0, 1)]))
                    {
                        return false;
                    }
                    break;
                }
                case 'p':  // Validate ports
                {
                    if (elem == t_list[-1])
                    {
                        return error("-p PORT", ArgType::flag);
                    }

                    // Parse port substrings
                    if (!set_ports(t_list[t_list.find(elem, 0, 1)]))
                    {
                        return false;
                    }
                    break;
                }
                case 'u':  // Validate HTTP URI
                {
                    if (elem == t_list[-1])
                    {
                        return error("--uri URI", ArgType::flag);
                    }

                    // Parse/validate URI
                    if (!set_uri(t_list[t_list.find(elem, 0, 1)]))
                    {
                        return false;
                    }
                    break;
                }
                default:   // Unrecognized alias
                {
                    return errorf("Unrecognized flag: '%'", elem);
                }
            }
        }
        t_list.remove(elem);
    }
    return true;
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
            return help();
        }

        // Enable verbose console output
        if (elem == "--verbose")
        {
            args.verbose = true;
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

        // Validate output file path
        if (elem == "--output")
        {
            if (elem == t_list[-1])
            {
                return error("--output PATH", ArgType::flag);
            }

            if (!set_path(t_list[t_list.find(elem, 0, 1)]))
            {
                return false;
            }
            t_list.remove(elem);
            continue;
        }

        // Validate connection timeout
        if (elem == "--timeout")
        {
            if (elem == t_list[-1])
            {
                return error("--timeout MS", ArgType::flag);
            }

            if (!set_timeout(t_list[t_list.find(elem, 0, 1)]))
            {
                return false;
            }
            t_list.remove(elem);
            continue;
        }

        // Specify HTTP request URI
        if (elem == "--uri")
        {
            if (elem == t_list[-1])
            {
                return error("--uri URI", ArgType::flag);
            }

            if (!set_uri(t_list[t_list.find(elem, 0, 1)]))
            {
                return false;
            }
            t_list.remove(elem);
            continue;
        }

        // Validate ports
        if (elem == "--port")
        {
            if (elem == t_list[-1])
            {
                return error("--port PORT", ArgType::flag);
            }

            // Parse/validate port substrings
            if (!set_ports(t_list[t_list.find(elem, 0, 1)]))
            {
                return false;
            }
            t_list.remove(elem);
            continue;
        }

        // Unrecognized flag(s) received
        return errorf("Unrecognized flag: '%'", elem);
    }
    return true;
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
            case PathInfo::empty:             // Empty file path
                errorf("File path cannot lead to a directory: '%'", t_path);
                break;
            case PathInfo::directory:         // Path leads to directory
                errorf("File path cannot lead to a directory: '%'", t_path);
                break;
            case PathInfo::parent_not_found:  // Nonexistent parent path
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
* @brief  Parse and validate the given port or port range and update
*         the underlying command-line arguments.
*/
bool scan::ArgParser::set_ports(const string &t_ports)
{
    bool valid_ports{ !t_ports.empty() };

    // Validate port numbers
    for (const string &port : Util::split(t_ports, ","))
    {
        // Validate port (range validation occurs later)
        if (!is_port_range(port))
        {
            if (!net::valid_port(port))
            {
                valid_ports = errorf("'%' is not a valid port number", port);
                break;
            }

            args.ports.add(std::stoi(port));
            continue;
        }

        const vector<string> port_vect{ Util::split(port, "-") };

        List<int> port_range = List<int>::fill(std::stoi(port_vect[0]),
                                               std::stoi(port_vect[1]));

        // Validate port ranges
        for (const int &port_num : port_range)
        {
            // Skip '0' when used in port range
            if (port_num == 0)
            {
                continue;
            }

            // Invalid port received
            if (!net::valid_port(port_num))
            {
                valid_ports = errorf("'%' is not a valid port number", port_num);
                break;
            }
            args.ports.add(port_num);
        }
    }

    if (valid_ports)
    {
        m_argv.remove(t_ports);
    }
    return valid_ports;
}

/**
* @brief  Parse and validate the given socket timeout (in milliseconds)
*         and update the underlying command-line arguments.
*/
bool scan::ArgParser::set_timeout(const string &t_ms)
{
    bool valid_timeout{ true };

    // Update the connection timeout
    if (Util::is_integral(t_ms))
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
    bool valid_uri;
    string uri_str{ t_uri };

    // Ensure URI begins with '/'
    if (!uri_str.empty())
    {
        uri_str = (t_uri[0] == '/') ? t_uri : Util::fstr("/%", t_uri);
    }

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
    valid = parse_aliases(t_list) && parse_flags(t_list);

    // Validate remaining arguments
    if (valid)
    {
        switch (t_list.size())
        {
            case 0:   // Missing TARGET
            {
                error("TARGET", ArgType::value);
                break;
            }
            case 1:   // Syntax: TARGET
            {
                if (args.ports.empty())
                {
                    error("PORT", ArgType::value);
                    break;
                }
                args.target = t_list[0];
                break;
            }
            case 2:   // Syntax: TARGET PORTS
            {
                if (!set_ports(t_list[1]))
                {
                    valid = false;
                    break;
                }
                args.target = t_list[0];
                break;
            }
            default:  // Unrecognized argument
            {
                errorf("Failed to validate: '%'", t_list.join(", "));
                break;
            }
        }

        // Validate the target hostname/address
        if (valid && !args.target.is_valid())
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
    return valid;
}

/**
* @brief  Write the application usage information and the given network
*         socket error to the standard error stream.
*/
std::string scan::ArgParser::error(const error_code &t_ecode)
{
    valid = false;
    std::cout << m_usage << LF;

    const string error_msg{ net::error(args.target.name(), t_ecode) };
    std::cout << LF;

    return error_msg;
}
