/*
*  parser.cpp
*  ----------
*  Source file for command-line argument parser and validator
*/
#include <iostream>
#include <sstream>
#include "includes/except/nullptrex.h"
#include "includes/net/socket.h"
#include "includes/parser.h"

namespace Scan
{
    using string = std::string;
}

Scan::AutoProp<bool> Scan::Parser::verbose(false);

/// ***
/// Initialize the object
/// ***
Scan::Parser::Parser(const int &argc, const char *argv[])
{
    this->m_usage = (string("Usage: ") + EXE + " [OPTIONS] TARGET");

    this->addr = &m_addr;
    this->ports = &m_ports;
    this->valid = false;

    this->parse(argc, argv);
}

/// ***
/// Display application usage information
/// ***
void Scan::Parser::help() const
{
    std::stringstream ss;  // Usage info

    ss << "SvcScan (https://github.com/vandavey/SvcScan)" << LF
        << m_usage << LF << LF
        << "TCP socket application banner grabber" << LF << LF
        << "Positional Arguments:" << LF
        << "  TARGET            Target address or domain name" << LF << LF
        << "Optional Arguments:" << LF
        << "  -p/--port PORT    Port(s) - comma separated (no spaces)" << LF
        << "  -h/--help         Show this help message and exit" << LF
        << "  -v/--verbose      Enable verbose console output" << LF;

    std::cout << ss.str() << LF;
}

/// ***
/// Print usage and command-line argument error to stderr
/// ***
void Scan::Parser::error(const string &arg, const ArgType &argt) const
{
    string msg;

    switch (argt)
    {
        case ArgType::flag:    // Argument flag
        {
            msg = "Missing flag for optional argument(s): %\n";
            break;
        }
        case ArgType::value:  // Argument value
        {
            msg = "Missing value for argument(s): %\n";
            break;
        }
    }
    error(msg, arg);
}

/// ***
/// Print usage and command-line argument error to stderr
/// ***
void Scan::Parser::error(const string &msg, const string &arg) const
{
    std::cout << m_usage << LF;
    Util::errorf(msg, arg);
}

/// ***
/// Parse and validate command-line arguments
/// ***
void Scan::Parser::parse(const uint &argc, const char *argv[])
{
    if (argc == NULL)
    {
        throw NullArgEx("argc");
    }

    if (argv == nullptr)
    {
        throw NullPtrEx("argv");
    }

    // Show usage information
    if (argc == 1)
    {
        help();
        return;
    }

    // Parse arguments (exclude program path)
    for (uint i = {1}; i < argc; i++)
    {
        if (argv[i] != nullptr)
        {
            m_argv += argv[i];
        }
    }

    if (m_argv.any({"-h", "--help"}))
    {
        help();
        return;
    }
    validate(m_argv);
}

/// ***
/// Validate arguments parsed from cmd-line
/// ***
void Scan::Parser::validate(list_s &list)
{
    const size_t vsize = {list.size()};

    // Invalid vector size
    if ((vsize < 2) || (vsize > 4))
    {
        error("TARGET", ArgType::value);
        return;
    }
    valid = parse_aliases(list) && parse_flags(list);

    // Invalid arguments parsed
    if (!valid.get())
    {
        valid = false;
        return;
    }

    // Validate remaining arguments
    switch (list.size())
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
            m_addr = list[0];
            break;
        }
        case 2:   // Format: [TARGET PORTS]
        {
            if (!parse_ports(list[1]))
            {
                valid = false;
                return;
            }

            m_addr = list[0];
            break;
        }
        default:  // Unrecognized arguments
        {
            valid = false;
            error("Failed to validate: %", list.join(", "));;
            return;
        }
    }

    // Validate IPv4 address
    if (Socket::valid_ip(m_addr) == 1)
    {
        valid = false;
        error("'%' is not a valid IPv4 address", m_addr);
        return;
    }
}

/// ***
/// Parse and validate cmd-line flag aliases and values
/// ***
const bool Scan::Parser::parse_aliases(list_s &list)
{
    if (list.empty())
    {
        return false;
    }
    const vector_s clone(list);

    // Validate arg aliases and values
    for (const string &elem : clone)
    {
        // Skip non-alias arguments
        if ((elem.size() < 2) || (elem[0] != '-') || (elem[1] == '-'))
        {
            continue;
        }
        bool skip = {false};

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
                    if (elem == list.last())
                    {
                        error("PORT", ArgType::value);
                        return false;
                    }

                    const size_t index = list.index_of(elem);

                    // Parse/validate port substrings
                    if (!parse_ports(list[list.index_of(elem) + 1]))
                    {
                        return false;
                    }
                    break;
                }
                default:   // Unrecognized alias
                {
                    error("Unrecognized flag: '%'", elem);
                    return false;
                }
            }
        }
        list.remove(elem);
    }
    return true;
}

/// ***
/// Parse and validate cmd-line flags and values
/// ***
const bool Scan::Parser::parse_flags(list_s &list)
{
    if (list.empty())
    {
        return false;
    }
    const vector_s clone(list);

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
            list.remove(elem);
            continue;
        }

        // Validate ports
        if (elem == "--port")
        {
            // Argument value index out of range
            if (elem == list.last())
            {
                error("PORT", ArgType::value);
                return false;
            }

            // Parse/validate port substrings
            if (!parse_ports(list[list.index_of(elem) + 1]))
            {
                return false;
            }
            list.remove(elem);
            return true;
        }

        // Unrecognized flag(s) received
        error("Unrecognized flag: '%'", elem);
        return false;
    }
    return true;
}

/// ***
/// Extract port number substrings from comma delimited string
/// ***
const bool Scan::Parser::parse_ports(const string &ports)
{
    if (ports.empty())
    {
        return false;
    }

    // Validate port numbers
    for (const string &port : Util::split(ports, ","))
    {
        // Invalid port received
        if (!Socket::valid_port(port))
        {
            error("'%' is not a valid port", port);
            return false;
        }
        m_ports += port;
    }

    m_argv.remove(ports);
    return true;
}
