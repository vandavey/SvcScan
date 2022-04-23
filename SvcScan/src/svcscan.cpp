/*
*  svcscan.cpp
*  -----------
*  Source file containing application entry point (main)
*/
#ifndef UNICODE
#  define UNICODE 1
#endif // !UNICODE

#include "includes/inet/scanner.h"

/// ***
/// Static application entry point
/// ***
int main(int argc, char *argv[])
{
    using namespace scan;
    using io_context = boost::asio::io_context;

    SetConsoleTitleA(Util::fstr("SvcScan (%)", ArgParser::REPO).c_str());

    // Enable virtual terminal sequences
    const int rcode{ StdUtil::enable_vt() };

    if (rcode != NO_ERROR)
    {
        StdUtil::warnf("Virtual terminal sequences are disabled: '%'", rcode);
    }

    ArgParser parser;
    int exit_code{ 1 };

    // Scan the specified target
    if (parser.parse_argv(argc, argv))
    {
        io_context ioc;
        Scanner scanner{ ioc, parser.args };

        parser.~ArgParser();

        try  // Run scan against target
        {
            scanner.scan();
            exit_code = 0;
        }
        catch (const Exception &ex)
        {
            ex.show();
        }
    }
    else if (parser.help_shown)
    {
        exit_code = 0;
    }

    return exit_code;
}
