/*
*  svcscan.cpp
*  -----------
*  Source file for the application entry point
*/
#include "includes/inet/scanners/tls_scanner.h"
#include "includes/utils/arg_parser.h"
#include "includes/svcscan.h"

/**
* @brief  Customize the console title and enable virtual terminal processing.
*/
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

/**
* @brief  Perform the service scan against the specified target.
*/
int scan::run_scan(io_context &t_ioc, const Args &t_args)
{
    int rcode{ 1 };

    unique_ptr<Scanner> scannerp;
    shared_ptr<Args> argsp{ std::make_shared<Args>(t_args) };

    // Use SSL/TLS capable scanner
    if (t_args.tls_enabled)
    {
        scannerp = std::make_unique<TlsScanner>(t_ioc, argsp);
    }
    else  // Use standard scanner
    {
        scannerp = std::make_unique<Scanner>(t_ioc, argsp);
    }

    try  // Run the network scan
    {
        scannerp->scan();
        rcode = NOERROR;
    }
    catch (const Exception &ex)
    {
        ex.show();
    }
    return rcode;
}

/**
* @brief  Static application entry point.
*/
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
        exit_code = run_scan(ioc, parser.args);
    }
    else if (parser.help_shown)
    {
        exit_code = NOERROR;
    }

    return exit_code;
}
