/*
*  svcscan.cpp
*  -----------
*  Source file for the application entry point
*/
#include "includes/inet/scanner.h"
#include "includes/utils/arg_parser.h"
#include "includes/svcscan.h"

/// ***
/// Configure various options for the current console
/// ***
void scan::setup_console()
{
    if (!SetConsoleTitleA(Util::fstr("SvcScan (%)", ArgParser::REPO).c_str()))
    {
        StdUtil::warnf("Failed to set console title: '%'", GetLastError());
    }
    const int rcode{ StdUtil::enable_vt() };

    if (rcode != NOERROR)
    {
        StdUtil::warnf("Virtual terminal processing is disabled: '%'", rcode);
    }
}

/// ***
/// Perform the service scan against the specified target
/// ***
int scan::perform_scan(io_context &t_ioc, const Args &t_args)
{
    int rcode{ 1 };
    Scanner scanner{ t_ioc, t_args };

    try  // Scan the specified target
    {
        scanner.scan();
        rcode = NOERROR;
    }
    catch (const Exception &ex)
    {
        ex.show();
    }
    return rcode;
}

/// ***
/// Static application entry point
/// ***
int main(int argc, char *argv[])
{
    using namespace scan;

    ArgParser parser;
    int exit_code{ 1 };

    setup_console();

    // Scan the specified target
    if (parser.parse_argv(argc, argv))
    {
        io_context ioc;
        exit_code = perform_scan(ioc, parser.args);
    }
    else if (parser.help_shown)
    {
        exit_code = NOERROR;
    }

    return exit_code;
}
