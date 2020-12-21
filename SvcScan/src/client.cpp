#include "../include/client.h"

#ifdef _WIN32
#else
#  include <unistd.h>
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#endif// _WIN32

namespace Scan
{
    using std::string;
    using std::wstring;
}

/// ***
/// Initialize the object
/// ***
Scan::Client::Client()
{
    this->m_addr = "127.0.0.1";
    this->addr = &m_addr;
    this->ports = &m_ports;
}

/// ***
/// Initialize the object
/// ***
Scan::Client::Client(const Client &client)
{
    this->swap(client);
}

/// ***
/// Initialize the object
/// ***
Scan::Client::Client(const Property<string> &addr,
                     const Property<vector_s> &ports) {
    this->swap(addr, ports);
}

/// ***
/// Destroy the object
/// ***
Scan::Client::~Client()
{
#ifdef WIN_OS
    WSACleanup();
#endif// WIN_OS
}

/// ***
/// Assignment operator overload
/// ***
Scan::Client &Scan::Client::operator=(const Client &client)
{
    return swap(client);
}

/// ***
/// Determine if port string is a valid network port
/// ***
const bool Scan::Client::valid_port(const string &port) const
{
    // Ensure port can be parsed as integer
    for (const uchar &ch : port)
    {
        if (!std::isdigit(ch))
        {
            return false;
        }
    }
    const int iport = {std::stoi(port)};
    return ((iport >= 0) && (iport <= 65535));
}

/// ***
/// Determine if socket is valid
/// ***
const bool Scan::Client::valid_sock(const SOCKET &sock) const
{
    if ((sock == INVALID_SOCKET) || (sock == SOCKET_ERROR))
    {
        return false;
    }
    return true;
}

/// ***
/// Convert long long integer into a char pointer
/// ***
const char *Scan::Client::itoc(const llong &num) const
{
    if (num == NULL)
    {
        Style::error("Numeric value cannot be NULL");
    }
    const string num_s(itos(num));
    return num_s.empty() ? nullptr : num_s.c_str();
}

/// ***
/// Convert an integer into a string
/// ***
const std::string Scan::Client::itos(const llong &num) const
{
    return std::to_string(num);
}

/// ***
/// Get a reference to the abstract base class
/// ***
const Scan::Client &Scan::Client::base() const
{
    return *this;
}

/// ***
/// Swap mutable member values with reference object values
/// ***
Scan::Client &Scan::Client::swap(const Client &client)
{
    this->m_addr = client.m_addr;
    this->m_ports = client.m_ports;
    this->m_services = client.m_services;

    this->addr = &m_addr;
    this->ports = &m_ports;

    return *this;
}

/// ***
/// Swap mutable member values with reference object values
/// ***
Scan::Client &Scan::Client::swap(const Property<std::string> &addr,
                                 const Property<vector_s> &ports) {
    // Missing address
    if (addr.empty())
    {
        Style::error("Target address cannot be empty");
        return *this;
    }
    this->m_addr = addr.get();
    this->addr = &m_addr;

    // Missing port(s)
    if (ports.empty())
    {
        Style::error("Target ports cannot be empty");
        return *this;
    }
    this->m_ports = ports.get();
    this->ports = &m_ports;

    return *this;
}
