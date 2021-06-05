/*
*  parser.cpp
*  ----------
*  Source file for command-line argument parser and validator
*/
#include <iostream>
#include "includes/container/range.h"
#include "includes/except/nullptrex.h"
#include "includes/inet/socket.h"
#include "includes/utils/parser.h"

/// ***
/// Command-line argument enumeration type
/// ***
enum class scan::Parser::ArgType : short
{
    flag,  // Syntax: -f, --foo
    value  // Syntax: --foobar <value>
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
    vector_s usage_lines
    {
        "SvcScan (https://github.com/vandavey/SvcScan)",
        m_usage + LF,
        "TCP socket application banner grabber\n",
        "Positional Arguments:",
        "  TARGET                  Target address or domain name\n",
        "Optional Arguments:",
        "  -h/-?,   --help         Show this help message and exit",
        "  -v,      --verbose      Enable verbose console output",
        "  -p PORT, --port PORT    Port(s) - comma separated (no spaces)\n",
        "Usage Examples:",
        "  svcscan.exe -p 22-25,53 192.168.1.1",
        "  svcscan.exe -v localhost 21,443,80",
    };

    std::cout << List<string>::join(usage_lines) << LF << LF;
}

/// ***
/// Print usage and command-line argument error to stderr
/// ***
void scan::Parser::error(const string &t_arg, const ArgType &t_arg_type) const
{
    // Missing flag argument
    if (t_arg_type == ArgType::flag)
    {
        errorf("Missing flag argument: '%'", t_arg);
        return;
    }

    // Missing positional argument
    errorf("Missing required argument(s): '%'", t_arg);
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
    valid = parse_aliases(t_list) && parse_flags(t_list);

    // Invalid arguments parsed
    if (!valid)
    {
        return;
    }

    // Validate remaining arguments
    switch (t_list.size())
    {
        case 0:   // Missing TARGET
        {
            valid = false;
            error("TARGET", ArgType::value);
            return;
        }
        case 1:   // Syntax: TARGET
        {
            if (m_ports.empty())
            {
                valid = false;
                error("PORT", ArgType::value);
                return;
            }
            m_addr = t_list[0];
            break;
        }
        case 2:   // Syntax: TARGET PORTS
        {
            if (!parse_ports(t_list[1]))
            {
                valid = false;
                return;
            }
            m_addr = t_list[0];
            break;
        }
        default:  // Unrecognized arguments
        {
            valid = false;
            errorf("Failed to validate: '%'", t_list.join(", "));;
            return;
        }
    }

    // Validate IPv4 address
    if (Socket::valid_ip(m_addr) == 1)
    {
        valid = false;
        errorf("'%' is not a valid IPv4 address", m_addr);
        return;
    }
}

/// ***
/// Parse and validate cmd-line flag aliases and values
/// ***
bool scan::Parser::parse_aliases(list_s &t_list)
{
    if (t_list.empty())
    {
        return true;
    }
    const vector_s clone{ t_list };

    // Validate arg aliases and values
    for (const string &elem : clone)
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
                case 'p':  // Validate ports
                {
                    // Arg value index out of range
                    if (elem == t_list.last())
                    {
                        error("-p PORT", ArgType::flag);
                        return false;
                    }
                    size_t idx{ static_cast<size_t>(t_list.index_of(elem)) };

                    // Parse/validate port substrings
                    if (!parse_ports(t_list[idx + 1]))
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
    if (t_list.empty())
    {
        return true;
    }
    const vector_s clone{ t_list };

    // Validate arg flags and values
    for (const string &elem : clone)
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

        // Validate ports
        if (elem == "--port")
        {
            // Argument value index out of range
            if (elem == t_list.last())
            {
                error("--port PORT", ArgType::flag);
                return false;
            }

            // Parse/validate port substrings
            if (!parse_ports(t_list[t_list.index_of(elem) + 1]))
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
/// Extract port number substrings from comma delimited string
/// ***
bool scan::Parser::parse_ports(const string &t_ports)
{
    if (t_ports.empty())
    {
        return false;
    }

    // Validate port numbers
    for (const string &port : Util::split(t_ports, ","))
    {
        // Validate individual ports
        if (port.find("-") == string::npos)
        {
            if (!Socket::valid_port(port))
            {
                errorf("'%' is not a valid port", port);
                return false;
            }
            m_ports.add(std::stoi(port));
            continue;
        }

        const vector_s ports{ Util::split(port, "-") };
        const range_i range(std::stoi(ports[0]), std::stoi(ports[1]));

        // Validate port ranges
        for (const int iport : range)
        {
            // Invalid port received
            if (!Socket::valid_port(iport))
            {
                errorf("'%' is not a valid port", iport);
                return false;
            }
            m_ports.add(iport);
        }
    }

    m_argv.remove(t_ports);
    return true;
}
