/*
*  parser.cpp
*  ----------
*  Source file for command-line argument parser and validator
*/
#include <algorithm>
#include <cmath>
#include <iostream>
#include "includes/containers/range.h"
#include "includes/except/nullptrex.h"
#include "includes/filesys/filestream.h"
#include "includes/inet/socket.h"
#include "includes/utils/parser.h"

/// ***
/// Command-line argument enumeration type
/// ***
enum class scan::Parser::ArgType : short
{
    unknown,  // Unknown argument
    flag,     // Syntax: -f, --foo
    value     // Syntax: --foobar <value>
};

scan::AutoProp<bool> scan::Parser::verbose{ false };

/// ***
/// Initialize the object
/// ***
scan::Parser::Parser(const int &t_argc, char *t_argv[])
{
    m_usage = string("Usage: ") + EXE + " [OPTIONS] TARGET";

    addr = &m_addr;
    ports = &m_ports;
    help_shown = false;
    valid = false;

    parse(t_argc, t_argv);
}

/// ***
/// Display application usage information
/// ***
void scan::Parser::help()
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
    std::cout << list_s::join(usage_lines) << LF << LF;
}

/// ***
/// Print usage and command-line argument error to stderr
/// ***
void scan::Parser::error(const string &t_arg,
                         const ArgType &t_arg_type,
                         const bool &t_valid) {
    switch (t_arg_type)
    {
        case ArgType::unknown:  // Unknown argument
            errorf("Unable to validate argument: '%'", t_arg, t_valid);
            break;
        case ArgType::flag:     // Missing flag
            errorf("Missing flag argument: '%'", t_arg, t_valid);
            break;
        case ArgType::value:    // Missing value
            errorf("Missing required argument(s): '%'", t_arg, t_valid);
            break;
        default:
            break;
    }
}

/// ***
/// Parse and validate command-line arguments
/// ***
void scan::Parser::parse(const uint &t_argc, char *t_argv[])
{
    if ((t_argc == NULL) || (t_argv == nullptr))
    {
        throw t_argc ? NullPtrEx{ "t_argv" } : NullArgEx{ "t_argc" };
    }

    // Show usage information
    if (t_argc == 1)
    {
        help();
        return;
    }

    // Parse arguments (exclude program path)
    for (uint i{ 1 }; i < t_argc; i++)
    {
        if (t_argv[i] != nullptr)
        {
            m_argv.add(t_argv[i]);
        }
    }

    if (m_argv.any({ "-?", "-h", "--help" }))
    {
        help();
        return;
    }
    validate(m_argv);
}

/// ***
/// Validate arguments parsed from cmd-line
/// ***
void scan::Parser::validate(list_s &t_list)
{
    // Invalid arguments parsed
    if (valid = parse_aliases(t_list) && parse_flags(t_list); !valid)
    {
        return;
    }

    // Validate remaining arguments
    switch (t_list.size())
    {
        case 0:   // Missing TARGET
        {
            error("TARGET", ArgType::value);
            return;
        }
        case 1:   // Syntax: TARGET
        {
            if (m_ports.empty())
            {
                error("PORT", ArgType::value);
                return;
            }
            m_addr = t_list[0];
            break;
        }
        case 2:   // Syntax: TARGET PORTS
        {
            if (!set_ports(t_list[1]))
            {
                valid = false;
                return;
            }
            m_addr = t_list[0];
            break;
        }
        default:  // Unrecognized argument
        {
            errorf("Failed to validate: '%'", t_list.join(", "));
            return;
        }
    }

    // Validate IPv4 address
    if (net::valid_ip(m_addr) == 1)
    {
        errorf("'%' is not a valid IPv4 address", m_addr);
    }
}

/// ***
/// Parse and validate cmd-line flag aliases and values
/// ***
bool scan::Parser::parse_aliases(list_s &t_list)
{
    if (t_list.contains("-"))
    {
        error("-", ArgType::unknown);
        return false;
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
                    help();
                    return false;
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
                        error("-o PATH", ArgType::flag);
                        return false;
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
                        error("-t MS", ArgType::flag);
                        return false;
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
                        error("-p PORT", ArgType::flag);
                        return false;
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
                    errorf("Unrecognized flag: '%'", elem);
                    return false;
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
bool scan::Parser::parse_flags(list_s &t_list)
{
    if (t_list.contains("--"))
    {
        error("--", ArgType::unknown);
        return false;
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
            help();
            return false;
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
                error("--output PATH", ArgType::flag);
                return false;
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
                error("--timeout MS", ArgType::flag);
                return false;
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
                error("--port PORT", ArgType::flag);
                return false;
            }

            // Parse/validate port substrings
            if (!set_ports(t_list[t_list.find(elem, 0, 1)]))
            {
                return false;
            }
            t_list.remove(elem);
            return true;
        }

        // Unrecognized flag(s) received
        errorf("Unrecognized flag: '%'", elem);
        return false;
    }
    return true;
}

/// ***
/// Extract output file path and resolve if relative
/// ***
bool scan::Parser::set_path(const string &t_path)
{
    const bool valid_path{ Path::valid_file(t_path) };

    // Handle valid/invalid paths separately
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
        Socket::out_path = Path::resolve(t_path);
        m_argv.remove(t_path);
    }
    return valid_path;
}

/// ***
/// Extract port number substrings from comma delimited string
/// ***
bool scan::Parser::set_ports(const string &t_ports)
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
                errorf("'%' is not a valid port", port);
                break;
            }
            m_ports.add(std::stoi(port));
            continue;
        }

        const vector_s ports{ Util::split(port, "-") };
        const range_i range{ std::stoi(ports[0]), std::stoi(ports[1]) };

        // Validate port ranges
        for (const int iport : range)
        {
            // Invalid port received
            if (!net::valid_port(iport))
            {
                errorf("'%' is not a valid port", iport);
                break;
            }
            m_ports.add(iport);
        }
    }

    if (valid_ports)
    {
        m_argv.remove(t_ports);
    }
    return valid_ports;
}

/// ***
/// Extract the socket timeout in milliseconds
/// ***
bool scan::Parser::set_timeout(const string &t_ms)
{
    const bool is_digit = std::all_of(t_ms.begin(), t_ms.end(), [](const char &l_ch)
    {
        return std::isdigit(l_ch);
    });

    // Value must be an integer
    if (!is_digit)
    {
        errorf("'%' is not a valid connection timeout", t_ms);
        return false;
    }

    const double total_ms{ std::abs(std::stod(t_ms)) };
    const double total_sec{ total_ms / 1000 };

    const int sec{ static_cast<int>(std::floor(total_sec)) };
    const int ms{ static_cast<int>((total_sec - sec) * 1000) };

    // Update the default socket timeout
    Socket::set_timeout(sec, ms);

    m_argv.remove(t_ms);
    return true;
}
