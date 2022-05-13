/*
*  svcscan.cpp
*  -----------
*  Source file containing application entry point (main)
*/
#ifndef UNICODE
#  define UNICODE 1
#endif // !UNICODE

#include "includes/inet/scanner.h"
#include "includes/utils/arg_parser.h"

/// ***
/// Static application entry point
/// ***
int main(int argc, char *argv[])
{
    using namespace scan;
    using io_context = boost::asio::io_context;

    SetConsoleTitleA(Util::fstr("SvcScan (%)", ArgParser::REPO).c_str());

    // Enable virtual terminal processing
    const int rcode{ StdUtil::enable_vt() };

    if (rcode != NOERROR)
    {
        StdUtil::warnf("Virtual terminal processing is disabled: '%'", rcode);
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
            exit_code = NOERROR;
        }
        catch (const Exception &ex)
        {
            ex.show();
        }
    }
    else if (parser.help_shown)
    {
        exit_code = NOERROR;
    }

    return exit_code;
}
