/*
*  parser.cpp
*  ----------
*  Source file for command-line argument parser and validator
*/
#include <algorithm>
#include <cmath>
#include <iostream>
#include "includes/containers/generic/range.h"
#include "includes/except/nullptrex.h"
#include "includes/filesys/filestream.h"
#include "includes/inet/scanner.h"
#include "includes/utils/argparser.h"

/// ***
/// Command-line argument enumeration type
/// ***
enum class scan::ArgParser::ArgType : short
{
    unknown,  // Unknown argument
    flag,     // Syntax: -f, --foo
    value     // Syntax: --foobar <value>
};

bool scan::ArgParser::verbose{ false };

/// ***
/// Initialize the object
/// ***
scan::ArgParser::ArgParser()
{
    m_usage = string("Usage: ") + EXE + " [OPTIONS] TARGET";
    valid = help_shown = false;
}

/// ***
/// Display application usage information
/// ***
bool scan::ArgParser::help()
{
    help_shown = true;

    // Usage information
    const vector_s usage_lines
    {
        Util::fstr("SvcScan (%)", REPO),
        m_usage + LF,
        "TCP socket application banner grabber\n",
        "Positional Arguments:",
        "  TARGET                    Target address or domain name\n",
        "Optional Arguments:",
        "  -h/-?,   --help           Show this help message and exit",
        "  -v,      --verbose        Enable verbose console output",
        "  -p PORT, --port PORT      Port(s) - comma separated (no spaces)",
        "  -t MS,   --timeout MS     Connection timeout (milliseconds)",
        "                            [Default: 3500]",
        "  -o PATH, --output PATH    Write scan output to text file\n",
        "Usage Examples:",
        "  svcscan.exe -v localhost 21,443,80",
        "  svcscan.exe -p 22-25,53 192.168.1.1",
        "  svcscan.exe -vt 500 192.168.1.1 4444"
    };

    std::cout << list_s::join_lines(usage_lines) << LF << LF;
    return false;
}

/// ***
/// Parse and validate command-line arguments
/// ***
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
    if ((t_argc == 1) || m_argv.any({ "-?", "-h", "--help" }))
    {
        show_help = true;
    }

    return show_help ? help() : validate(m_argv);
}

/// ***
/// Print usage and command-line argument error to stderr
/// ***
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

/// ***
/// Parse and validate cmd-line flag aliases and values
/// ***
bool scan::ArgParser::parse_aliases(list_s &t_list)
{
    if (t_list.contains("-"))
    {
        return error("-", ArgType::unknown);
    }

    // Validate arg aliases and values
    for (const string &elem : vector_s{ t_list })
    {
        // Skip non-alias arguments
        if ((elem.size() < 2) || (elem[0] != '-') || (elem[1] == '-'))
        {
            continue;
        }

        // Validate cmd-line arg aliases (-<alias>)
        for (const char &ch : elem)
        {
            switch (ch)
            {
                case '-':  // Ignore character
                {
                    break;
                }
                case 'h':  // Show usage information
                {
                    return help();
                }
                case 'v':  // Enable verbose output
                {
                    verbose = true;
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

/// ***
/// Parse and validate cmd-line flags and values
/// ***
bool scan::ArgParser::parse_flags(list_s &t_list)
{
    if (t_list.contains("--"))
    {
        return error("--", ArgType::unknown);
    }

    // Validate arg flags and values
    for (const string &elem : vector_s{ t_list })
    {
        // Skip non-flag arguments
        if ((elem.size() < 3) || (elem.rfind("--") != 0))
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
            verbose = true;
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

/// ***
/// Extract output file path and resolve if relative
/// ***
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
        Scanner::out_path = Path::resolve(t_path);
        m_argv.remove(t_path);
    }
    return valid_path;
}

/// ***
/// Extract port number substrings from comma delimited string
/// ***
bool scan::ArgParser::set_ports(const string &t_ports)
{
    bool valid_ports{ !t_ports.empty() };

    // Validate port numbers
    for (const string &port : Util::split(t_ports, ","))
    {
        // Validate individual ports
        if (port.find("-") == string::npos)
        {
            if (!net::valid_port(port))
            {
                valid_ports = errorf("'%' is not a valid port", port);
                break;
            }
            ports.add(std::stoi(port));
            continue;
        }

        const vector_s port_vect{ Util::split(port, "-") };
        const range_i range{ std::stoi(port_vect[0]), std::stoi(port_vect[1]) };

        // Validate port ranges
        for (const int &iport : range)
        {
            // Invalid port received
            if (!net::valid_port(iport))
            {
                valid_ports = errorf("'%' is not a valid port", iport);
                break;
            }
            ports.add(iport);
        }
    }

    if (valid_ports)
    {
        m_argv.remove(t_ports);
    }
    return valid_ports;
}

/// ***
/// Extract the socket timeout in milliseconds from the given string
/// ***
bool scan::ArgParser::set_timeout(const string &t_ms)
{
    bool valid_timeout{ true };

    // Update the connection timeout
    if (Util::is_integral(t_ms))
    {
        Scanner::connect_timeout(std::stoi(t_ms));
        m_argv.remove(t_ms);

        valid_timeout = true;
    }
    else  // Expected integral value
    {
        valid_timeout = errorf("'%' is not a valid connection timeout", t_ms);
    }
    return valid_timeout;
}

/// ***
/// Validate arguments parsed from cmd-line
/// ***
bool scan::ArgParser::validate(list_s &t_list)
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
                if (ports.empty())
                {
                    error("PORT", ArgType::value);
                    break;
                }
                addr = t_list[0];
                break;
            }
            case 2:   // Syntax: TARGET PORTS
            {
                if (!set_ports(t_list[1]))
                {
                    valid = false;
                    break;
                }
                addr = t_list[0];
                break;
            }
            default:  // Unrecognized argument
            {
                errorf("Failed to validate: '%'", t_list.join(", "));
                break;
            }
        }

        // Validate IPv4 address
        if (valid && net::valid_ipv4_fmt(addr) && !net::valid_ipv4(addr))
        {
            errorf("'%' is not a valid IPv4 address", addr);
        }
    }
    return valid;
}
