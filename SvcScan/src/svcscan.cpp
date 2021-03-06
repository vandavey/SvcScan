/*
*  svcscan.cpp
*  -----------
*  Source file containing application entry point (main)
*/
#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#include <iostream>
#include "includes/inet/socket.h"
#include "includes/utils/parser.h"
#include "includes/utils/util.h"

/// ***
/// Static application entry point
/// ***
int main(int argc, char *argv[])
{
    using namespace scan;

    // Enable virtual terminal sequences
    if (Util::enable_vt() != 0)
    {
        Util::warn("Virtual terminal sequences are disabled");
    }
    const Parser parser(argc, argv);

    // Invalid cmd-line arguments
    if (!parser.valid)
    {
        // Return error (1) if exception occurred
        return parser.help_shown ? 0 : 1;
    }

    Socket sock(parser.addr, parser.ports);
    parser.~Parser();

    sock.connect();
    sock.~Socket();

    return 0;
}
