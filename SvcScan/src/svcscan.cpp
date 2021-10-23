/*
*  svcscan.cpp
*  -----------
*  Source file containing application entry point (main)
*/
#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#include "includes/conio/stdutil.h"
#include "includes/inet/sockets/socket.h"
#include "includes/utils/parser.h"

/// ***
/// Static application entry point
/// ***
int main(int argc, char *argv[])
{
    using namespace scan;

    SetConsoleTitleA(Util::fstr("SvcScan (%)", Parser::REPO).c_str());

    // Enable virtual terminal sequences
    if (StdUtil::enable_vt() != 0)
    {
        StdUtil::warn("Virtual terminal sequences are disabled");
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
