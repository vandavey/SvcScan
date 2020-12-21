#include <iostream>
#include <string>
#include "include/parser.h"
#include "include/style.h"

#ifndef UNICODE
#  define UNICODE
#endif // !UNICODE

#ifdef _WIN32
#  ifndef WIN_OS
#    define WIN_OS
#  endif// !WIN_OS
#  include <conio.h>
#  include "include/winclient.h"
#else
#  ifndef NIX_OS
#    define NIX_OS
#  endif// !NIX_OS
#  include "include/nixclient.h"
#endif // _WIN32

using namespace Scan;

/// ***
/// Static application entry point
/// ***
int main(const int argc, const char *argv[])
{
    if (Style::enable_vtmode() != NO_ERROR)
    {
        Style::warning("VT escape sequence processing disabled");
    }

    const Parser parser(argc, argv);

    // Invalid cmd-line arguments
    if (!parser.valid)
    {
        if (argc == 1)
        {
            parser.show_help();
        }
        return (argc == 1) ? 0 : 1;
    }

#ifdef WIN_OS
    WinClient client(parser.addr, parser.ports);
#else
    NixClient client(parser.addr, parser.ports);
#endif // WIN_OS

    client.start();
    std::cout << std::endl;

#if defined(WIN_OS) && defined(_DEBUG)
    Style::print("[DEBUG]: Press any key to terminate...");
    short _ = _getch();
#endif // WIN_OS && _DEBUG

    return 0;
}
