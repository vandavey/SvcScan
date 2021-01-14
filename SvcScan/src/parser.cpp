/*
*  parser.cpp
*  ----------
*  Source file for command-line argument parser and validator
*/
#include <iostream>
#include <sstream>
#include "includes/except/nullargex.h"
#include "includes/net/socket.h"
#include "includes/parser.h"

namespace Scan
{
    using std::endl;
    using std::string;
}

/// ***
/// Initialize the object
/// ***
Scan::Parser::Parser()
{
    this->addr = &m_addr;
    this->ports = &m_ports;
    this->help_txt = false;
    this->valid = false;
}

/// ***
/// Initialize the object
/// ***
Scan::Parser::Parser(const int &argc, const char *argv[])
{
    this->m_usage = (string("Usage: ") + EXE + " [OPTIONS] TARGET");

    this->addr = &m_addr;
    this->ports = &m_ports;
    this->help_txt = false;
    this->valid = false;

    this->parse(argc, argv);
}

/// ***
/// Display application usage information
/// ***
void Scan::Parser::help() const
{
    std::stringstream ss;  // Usage info

    ss << "SvcScan (https://github.com/vandavey/SvcScan)" << endl
        << m_usage << endl << endl
        << "TCP socket application banner grabber" << endl << endl
        << "Positional Arguments:" << endl
        << "  TARGET            Target address or domain name" << endl << endl
        << "Optional Arguments:" << endl
        << "  -h/--help         Show this help message and exit" << endl
        << "  -p/--port PORT    Port(s) - comma separated (no spaces)" << endl;

    std::cout << ss.str() << endl;
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
        case ArgType::optval:  // Optional value
        {
            msg = "Missing value for optional argument(s): %\n";
            break;
        }
        case ArgType::reqval:  // Required value
        {
            msg = "Missing value for required argument(s): %\n";
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
    std::cout << m_usage << endl;
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
        throw NullArgEx("argv", true);
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
            m_argv.push_back(argv[i]);
        }
    }

    if (index('h', "help") != -1)
    {
        help();
        return;
    }
    validate(m_argv);
}

/// ***
/// Validate arguments parsed from cmd-line
/// ***
void Scan::Parser::validate(const vector_s &argv)
{  
    const int vsize = {static_cast<int>(m_argv.size())};

    // Invalid argument(s)
    if ((vsize != 2) && (vsize != 3))
    {
        error("TARGET, PORT", ArgType::reqval);
        return;
    }
    const int pindex = {index('p', "port")};

    // Check for unexpected [-p] in [TARGET PORT]
    if ((vsize == 2) && (pindex != -1))
    {
        if (pindex == 0)
        {
            error("TARGET", ArgType::reqval);
            return;
        }
        error("PORT", ArgType::optval);
        return;
    }
    string ports_arg;

    if (vsize == 3)
    {
        switch (pindex)
        {
            case -1:  // Missing PORT flag
            {
                error("--port", ArgType::flag);
                return;
            }
            case 0:   // Format: [-p PORTS TARGET]
            {
                m_addr = argv[2];
                ports_arg = argv[1];
                break;
            }
            case 1:   // Format: [TARGET -p PORTS]
            {
                m_addr = argv[0];
                ports_arg = argv[2];
                break;
            }
            case 2:   // Missing PORT value
            {
                error("PORT", ArgType::optval);
                return;
            }
        }
    }
    else  // Format: [TARGET PORTS]
    {
        m_addr = argv[0];
        ports_arg = argv[1];
    }

    // Validate IPv4 address
    if (Socket::valid_ip(m_addr) == 1)
    {
        error("'%' is not a valid IPv4 address", m_addr);
        return;
    }
    m_ports = Util::split(ports_arg, ',');
    ports = &m_ports;

    // Validate ports
    for (const string &port : m_ports)
    {
        if (!Socket::valid_port(port))
        {
            error("'%' is not a valid port", port);
            return;
        }
    }
    valid = (m_ports.size() > 0);
}

/// ***
/// Find index of a cmd-line argument in underlying vector
/// ***
const int Scan::Parser::index(const char &flag, const string &name) const
{
    if (flag == static_cast<const char &>(NULL))
    {
        throw NullArgEx("flag");
    }

    // No data to search
    if (name.empty())
    {
        return -1;
    }

    const string argflag(string("-") + flag);   // -<flag>
    const string argname(string("--") + name);  // --<name>

    // Find matching vector argument
    for (int i = {0}; i < m_argv.size(); i++)
    {
        if ((m_argv[i] == argflag) || (m_argv[i] == argname))
        {
            return i;
        }
    }
    return -1;
}
