#include <ws2tcpip.h>
#include "../include/winclient.h"

#pragma comment(lib, "Ws2_32.lib")

namespace Scan
{
    using std::string;
    using std::wstring;
}

/// ***
/// Initialize the object
/// ***
Scan::WinClient::WinClient()
{
}

/// ***
/// Initialize the object
/// ***
Scan::WinClient::WinClient(const WinClient &wc)
{
    this->operator=(wc);
}

/// ***
/// Initialize the object
/// ***
Scan::WinClient::WinClient(const Property<std::string> &addr,
                           const Property<vector_s> &ports) {
    this->swap(addr, ports);
}

/// ***
/// Destroy the object
/// ***
Scan::WinClient::~WinClient()
{
    WSACleanup();
}

/// ***
/// Assignment operator overload
/// ***
Scan::WinClient &Scan::WinClient::operator=(const WinClient &client)
{
    swap(client.base());
    return *this;
}

/// ***
/// Connect to the remote host
/// ***
void Scan::WinClient::start()
{
    for (const string &port : m_ports)
    {
        // Check for valid network port
        if (!valid_port(port))
        {
            Style::error("Invalid port: " + port);
            continue;
        }
        int code = {(int)SOCKET_ERROR};

        SOCKET sock = {INVALID_SOCKET};
        addrinfoW *ptr = {prep_sock(sock, port)};

        // Invalid socket descriptor
        if (!valid_sock(sock))
        {
            FreeAddrInfoW(ptr);
            continue;
        }
        code = set_sockopts(sock, {SO_RCVTIMEO, SO_SNDTIMEO});

        // Failed to set socket options
        if (code != NO_ERROR)
        {
            wsa_error();
            FreeAddrInfoW(ptr);
            continue;
        }
        fd_set fds = {1, {sock}};
        EndPoint ep(m_addr, port);

        // Put socket into non-blocking mode
        if ((code = sock_ioctl(sock, true)) != NO_ERROR)
        {
            wsa_error(ep.str());
            FreeAddrInfoW(ptr);
            continue;
        }
        code = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (code == SOCKET_ERROR)
        {
            // Connection failed
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                wsa_error(ep.str());
                FreeAddrInfoW(ptr);
                close_sock(sock);
                continue;
            }

            // Connection failed or timed out
            if (sock_select(nullptr, &fds) <= 0)
            {
                wsa_errorf(WSAEWOULDBLOCK, (string &)ep.str());
                FreeAddrInfoW(ptr);
                close_sock(sock);
                continue;
            }
        }
        char buffer[BUFFERSIZE] = {0};

        // Check socket readability
        switch (sock_select(&fds, nullptr))
        {
            /***
            * TODO: include domain name and addr in SvcInfo
            ***/
            case 1: // Readable
            {
                if ((code = recv(sock, buffer, BUFFERSIZE, 0)) > 0)
                {
                    SvcInfo si(ep, string(buffer, code));
                    std::cout << si << std::endl;
                    m_services.push_back(si);
                }
                break;
            }
            case NO_ERROR: // Timed out
            {
                Style::error("No data received from %", ep.str());
                break;
            }
            case SOCKET_ERROR: // Failure
            {
                wsa_error();
                break;
            }
        }
        shutdown(sock, SHUT_RDWR);
        close_sock(sock);
        FreeAddrInfoW(ptr);
    }
}

/// ***
/// Determine if IPv4 (dot-decimal notation) is valid
/// ***
const int Scan::WinClient::valid_ip(const string &addr) const
{
    size_t i, next = {0};
    int count = {0}, code = {(int)SOCKET_ERROR};

    while ((i = addr.find_first_not_of('.', next)) != -1)
    {
        if ((next = addr.find('.', i)) != -1)
        {
            count += 1;
        }
    }

    // Only validate if three '.' found
    if (count == 3)
    {
        const int size = {(int)sizeof(in_addr)};
        code = InetPtonW(AF_INET, utf16(addr).c_str(), (int *)&size);
        code = (code == 1) ? 0 : 1;
    }
    return code;
}

/// ***
/// Close socket and set its handle to default value
/// ***
void Scan::WinClient::close_sock(SOCKET &sock) const
{
    if (closesocket(sock) == SOCKET_ERROR)
    {
        wsa_error();
    }
    sock = INVALID_SOCKET;
}

/// ***
/// Print WSA error information to standard error
/// ***
void Scan::WinClient::wsa_error() const
{
    wsa_errorf(WSAGetLastError(), (string &)string());
}

/// ***
/// Print WSA error information to standard error
/// ***
void Scan::WinClient::wsa_error(const int &error) const
{
    wsa_errorf(error, (string &)string());
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void Scan::WinClient::wsa_error(const string &arg) const
{
    wsa_errorf(WSAGetLastError(), (string &)arg);
}

/// ***
/// Format and print a WSA error message to standard error
/// ***
void Scan::WinClient::wsa_errorf(const int &error, string &arg) const
{
    // Check for null error
    if (error == NULL)
    {
        Style::error("Winsock error code cannot be NULL");
        return;
    }
    arg = arg.empty() ? "destination host" : arg;

    switch (error)
    {
        case WSAENSLOOKUP: // DNS lookup error
        {
            Style::error("Cannot resolve %", arg);
            break;
        }
        case WSAEWOULDBLOCK: // Connection timed out
        {
            Style::error("Can't establish connection to %", arg);
            break;
        }
        case WSAETIMEDOUT: // Recv/send timed out
        {
            Style::error("Connection to % timed out", arg);
            break;
        }
        case WSAECONNREFUSED: // Connection refused
        {
            Style::error("Connection refused by %", arg);
            break;
        }
        case WSAEHOSTDOWN: // Destination host down
        {
            Style::error("% is down or unresponsive", arg);
            break;
        }
        case WSANOTINITIALISED: // WSAStartup call missing
        {
            Style::error("Missing call to WSAStartup");
            break;
        }
        default: // Default stderr
        {
            Style::error("Winsock error: " + itos(error));
            break;
        }
    }
}

/// ***
/// Set options on the socket descriptor
/// ***
template<size_t N>
const int Scan::WinClient::set_sockopts(SOCKET &sock,
                                        const int (&opts)[N]) const {
    const char *ptr = {itoc(3500)};
    int code = {(int)SOCKET_ERROR};

    if (ptr == nullptr)
    {
        return code;
    }
    const int len = {(int)sizeof(llong)};

    // Set socket options
    for (const int &opt : opts)
    {
        if (opt == NULL)
        {
            Style::error("Received a NULL socket option");
            continue;
        }
        code = setsockopt(sock, SOL_SOCKET, opt, ptr, len);

        if (code != NO_ERROR)
        {
            Style::error("Failed to set sockopt '%'", itos(opt));
        }
    }
    return code;
}

/// ***
/// Configure blocking options for the socket
/// ***
const int Scan::WinClient::sock_ioctl(SOCKET &sock,
                                      const bool &block) const {
    if (!valid_sock(sock))
    {
        Style::error("Invalid socket received");
        return -1;
    }
    ulong arg = block ? 1 : 0;
    return ioctlsocket(sock, FIONBIO, &arg);
}

/// ***
/// Determine if socket is readable or writeable
/// ***
const int Scan::WinClient::sock_select(fd_set *rfds_ptr, fd_set *wfds_ptr)
{
    if ((rfds_ptr == nullptr) && (wfds_ptr == nullptr))
    {
        return (int)SOCKET_ERROR;
    }
    const timeval to = {3, 500};
    return select(NULL, rfds_ptr, wfds_ptr, nullptr, &to);
}

/// ***
/// Transform UTF-16 encoding to UTF-8 encoding
/// ***
const std::string Scan::WinClient::utf8(const wstring &data_w) const
{
    if (data_w.empty())
    {
        return string();
    }
    const int len_w = {(int)data_w.size()};

    // Calculate required length
    int len = WideCharToMultiByte(CP_UTF8, 0, &data_w[0], len_w, NULL,
                                                          0, NULL, NULL);
    string data(len, 0);

    // Populate UTF-8 string
    WideCharToMultiByte(CP_UTF8, 0, data_w.c_str(), len_w, &data[0],
                                                    len, NULL, NULL);
    return data;
}

/// ***
/// Transform UTF-8 encoding to UTF-16 encoding
/// ***
const std::wstring Scan::WinClient::utf16(const string &data) const
{
    return Style::utf16(data);
}

/// ***
/// Prepare socket for connection to destination host
/// ***
addrinfoW *Scan::WinClient::prep_sock(SOCKET &sock,
                                      const string &port) const {
    int code;
    WSAData wsadata;

    // Intiialize Winsock2 DLL
    if ((code = WSAStartup(SOCKV, &wsadata)) == INVALID_SOCKET)
    {
        sock = INVALID_SOCKET;
        wsa_error();
        return nullptr;
    }

    addrinfoW *ptr, ai_hints = {
        AI_CANONNAME, AF_INET, SOCK_STREAM, IPPROTO_TCP
    };

    code = GetAddrInfoW(utf16(m_addr).c_str(), utf16(port).c_str(),
                                               &ai_hints, &ptr);
    // Check for DNS lookup errors
    if (code != 0)
    {
        sock = INVALID_SOCKET;
        wsa_error(m_addr);
        FreeAddrInfoW(ptr);
        return nullptr;
    }

    // Create connection sockeet
    sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    // Socket startup failure
    if (!valid_sock(sock))
    {
        sock = INVALID_SOCKET;
        wsa_error();
        FreeAddrInfoW(ptr);
        return nullptr;
    }
    return ptr;
}
