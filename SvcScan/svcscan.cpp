#include <iostream>
#include <string>
#include "include/parser.h"
#include "include/style.h"

#ifndef UNICODE
#  define UNICODE
#endif// !UNICODE

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
#endif// _WIN32

using namespace Scan;

namespace Scan
{
#ifdef _DEBUG
    void sandbox();
#endif// _DEBUG
}

#ifdef _DEBUG
/// ***
/// Conceptual sandbox debug function
/// ***
void Scan::sandbox()
{
    using std::endl;
    using std::string;
    typedef std::vector<string> vector_s;

    std::cout << "string: " << sizeof(string) << endl
              << "char *: " << sizeof(char *) << endl
              << "vector_s: " << sizeof(vector_s) << endl << endl
              << "sockaddr_in : " << sizeof(sockaddr_in) << endl
              << "in_addr : " << sizeof(in_addr) << endl << endl
              << "Property_s: " << sizeof(Property<string>) << endl
              << "Property_vs: " << sizeof(Property<vector_s>) << endl << endl
              << "AutoProp_s: " << sizeof(AutoProp<string>) << endl
              << "AutoProp_vs: " << sizeof(AutoProp<vector_s>) << endl << endl
              << "Style: " << sizeof(Style) << endl
              << "Client: " << sizeof(Client) << endl
              << "WinClient: " << sizeof(WinClient) << endl
              << "EndPoint: " << sizeof(EndPoint) << endl
              << "Parser: " << sizeof(Parser) << endl;
}
#endif// _DEBUG

/// ***
/// Application entry point
/// ***
int main(const int argc, const char *argv[])
{
    Style::enable_vtmode();
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
#endif// WIN_OS

    client.start();
    std::cout << std::endl;

#if defined(WIN_OS) && defined(_DEBUG)
    Style::print("[DEBUG]: Press any key to terminate...");
    short _ = _getch();
#endif// WIN_OS && _DEBUG

    return 0;
}
