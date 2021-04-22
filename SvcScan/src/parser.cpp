/*
*  parser.cpp
*  ----------
*  Source file for command-line argument parser and validator
*/
#include <iostream>
#include <sstream>
#include "includes/except/nullptrex.h"
#include "includes/net/socket.h"
#include "includes/utils/parser.h"

/// ***
/// Command-line argument enumeration type
/// ***
enum class scan::Parser::ArgType : short
{
    flag,  // -f, --flag
    value  // --flag value
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
    std::stringstream ss;  // Usage info

    ss << "SvcScan (https://github.com/vandavey/SvcScan)"              << LF
        << m_usage                                               << LF << LF
        << "TCP socket application banner grabber"               << LF << LF
        << "Positional Arguments:"                                     << LF
        << "  TARGET            Target address or domain name"   << LF << LF
        << "Optional Arguments:"                                       << LF
        << "  -p/--port PORT    Port(s) - comma separated (no spaces)" << LF
        << "  -h/--help         Show this help message and exit"       << LF
        << "  -v/--verbose      Enable verbose console output"         << LF;

    std::cout << ss.str() << LF;
}

/// ***
/// Print usage and command-line argument error to stderr
/// ***
void scan::Parser::error(const string &t_arg, const ArgType &t_arg_type) const
{
    switch (t_arg_type)
    {
        case ArgType::flag:   // Flag value
        {
            errorf("Missing flag argument: '%'", t_arg);
            break;
        }
        case ArgType::value:  // Argument value
        {
            errorf("Missing required argument(s): '%'", t_arg);
            break;
        }
        default:  // Invalid enum value
        {
            throw ArgEx("t_arg_type", "Invalid enumeration value");
        }
    }
}

/// ***
/// Print usage and a formatted argument error to stderr
/// ***
void scan::Parser::errorf(const string &t_msg, const string &t_arg) const
{
    std::cout << m_usage << LF;
    Util::errorf(t_msg, t_arg);
    std::cout << LF;
}

/// ***
/// Parse and validate command-line arguments
/// ***
void scan::Parser::parse(const uint &t_argc, char *t_argv[])
{
    if (t_argc == NULL)
    {
        throw NullArgEx("t_argc");
    }

    if (t_argv == nullptr)
    {
        throw NullPtrEx("t_argv");
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

    if (m_argv.any({ "-h", "--help" }))
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
        valid = false;
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
        case 1:   // Format: [TARGET]
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
        case 2:   // Format: [TARGET PORTS]
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
        // Invalid port received
        if (!Socket::valid_port(port))
        {
            errorf("'%' is not a valid port", port);
            return false;
        }
        m_ports.add(port);
    }

    m_argv.remove(t_ports);
    return true;
}
