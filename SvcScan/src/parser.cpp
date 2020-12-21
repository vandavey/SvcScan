#include <iostream>
#include <sstream>
#include "../include/parser.h"

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
    this->help = false;
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
    this->help = false;
    this->valid = false;

    this->parse(argc, argv);
}

/// ***
/// Display application usage information
/// ***
void Scan::Parser::show_help() const
{
    std::stringstream ss; // Usage info

    ss << "SvcScan (https://github.com/vandavey/SvcScan)" << endl
        << m_usage << endl << endl
        << "TCP socket application banner grabber" << endl << endl
        << "Positional Arguments:" << endl
        << "  TARGET            Target address or domain name" << endl << endl
        << "Optional Arguments:" << endl
        << "  -h/--help         Show this help message and exit" << endl
        << "  -p/--port PORT    Port(s) - comma separated (no spaces)" << endl;

    std::cout << ss.str();
}

/// ***
/// Print usage and command-line argument error to stderr
/// ***
void Scan::Parser::error(const string &arg, const ArgType &argt) const
{
    string msg;

    switch (argt)
    {
        case ArgType::flag: // Argument flag
        {
            msg = "Missing flag for optional argument(s): %";
            break;
        }
        case ArgType::optval: // Optional value
        {
            msg = "Missing value for optional argument(s): %";
            break;
        }
        case ArgType::reqval: // Required value
        {
            msg = "Missing value for required argument(s): %";
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
    std::cerr << m_usage << endl;
    Style::error(msg, arg);
}

/// ***
/// Parse and validate command-line arguments
/// ***
void Scan::Parser::parse(const uint &argc, const char *argv[])
{
    if ((argc == 0) || (argc == 1))
    {
        argc ? show_help() : Style::error("'argc' is invalid");
        return;
    }

    if (argv == nullptr)
    {
        Style::error("Invalid pointer received");
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

    // Check if [-h] flag was parsed
    if (this->index('h', "help") != -1)
    {
        this->show_help();
        return;
    }
    this->validate(m_argv);
}

/// ***
/// Validate arguments parsed from cmd-line
/// ***
void Scan::Parser::validate(const vector_s &argv)
{  
    const int vsize = {(int)m_argv.size()};

    if ((vsize != 2) && (vsize != 3))
    {
        error("TARGET, (PORT?)", ArgType::reqval);
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
            case -1: // Missing PORT flag
            {
                error("--port", ArgType::flag);
                return;
            }
            case 0: // fmt: [-p PORTS TARGET]
            {
                this->m_addr = argv[2];
                ports_arg = argv[1];
                break;
            }
            case 1: // fmt: [TARGET -p PORTS]
            {
                this->m_addr = argv[0];
                ports_arg = argv[2];
                break;
            }
            case 2: // Missing PORT
            {
                error("PORT", ArgType::optval);
                return;
            }
        }
    }
    else // Format: [TARGET PORTS]
    {
        this->m_addr = argv[0];
        ports_arg = argv[1];
    }

#ifdef WIN_OS
    WinClient client;
#else
    NixClient client;
#endif // WIN_OS

    // Validate IPv4 address if possible
    if (client.valid_ip(m_addr) == 1)
    {
        error("'%' is not a valid IPv4 address", m_addr);
        return;
    }
    this->m_ports = split(ports_arg, ',');

    // Validate network ports
    for (const string &port : m_ports)
    {
        if (!client.valid_port(port))
        {
            error("'%' is not a valid port", port);
            return;
        }
    }
    this->valid = (m_ports.size() > 0);
}

/// ***
/// Find index of a cmd-line argument in underlying vector
/// ***
const int Scan::Parser::index(const char &flag, const string &name) const
{
    if (flag == NULL)
    {
        Style::error("'flag' cannot be NULL");
        return -1;
    }

    if (name.empty())
    {
        Style::error("'name' cannot be empty");
        return -1;
    }

    const string argflag = {string("-") + flag}; // -<flag>
    const string argname = {string("--") + name}; // --<name>

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

/// ***
/// Split string by delimiter and return as a vector
/// ***
Scan::Parser::vector_s Scan::Parser::split(const string &data,
                                           const char &delim) const {
    if (data.empty())
    {
        return vector_s();
    }
    vector_s vect;

    // No delimiter to split on
    if (delim == NULL)
    {
        vect.push_back(data);
        return vect;
    }
    size_t i, next = {0};

    // Iterate until next delim not found
    while ((i = data.find_first_not_of(delim, next)) != -1)
    {
        next = data.find(delim, i);
        vect.push_back(data.substr(i, (next - i)));
    }
    return vect;
}

