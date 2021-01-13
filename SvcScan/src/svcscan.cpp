/*
*  svcscan.cpp
*  -----------
*  Source file containing application entry point (main) 
*/
#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#include <iostream>
#include "includes/net/socket.h"
#include "includes/parser.h"
#include "includes/util.h"

using namespace Scan;

/// ***
/// Static application entry point
/// ***
int main(const int argc, const char *argv[])
{
    if (Util::enable_vt() != static_cast<const int>(NO_ERROR))
    {
        Util::warn("VT escape sequence processing disabled");
    }
    const Parser parser(argc, argv);

    // Invalid cmd-line arguments
    if (!parser.valid)
    {
        if (argc == 1)
        {
            parser.help();
        }
        std::cout << std::endl;
        return (argc == 1) ? 0 : 1;
    }
    Socket sock(parser.addr, parser.ports);

    sock.connect();
    sock.~Socket();

    return 0;
}
