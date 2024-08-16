/*
* @file
*     main.cpp
* @brief
*     Source file for the application entry point.
*/
#include <memory>
#include "includes/console/arg_parser.h"
#include "includes/console/util.h"
#include "includes/errors/exception.h"
#include "includes/inet/scanners/tcp_scanner.h"
#include "includes/inet/scanners/tls_scanner.h"
#include "includes/main.h"
#include "includes/utils/aliases.h"
#include "includes/utils/const_defs.h"

/**
* @brief
*     Application entry point.
*/
int main(int argc, char* argv[])
{
    using namespace scan;
    setup_console();

    ArgParser parser;
    int exit_code{RCODE_ERROR};

    // Scan the specified target
    if (parser.parse_argv(argc, argv))
    {
        exit_code = run_scan(parser.args);
    }
    else if (parser.help_shown())
    {
        exit_code = RCODE_NO_ERROR;
    }

#ifdef _DEBUG
    {
        util::print("[DEBUG]: Press any key to terminate...");
        util::read_key();
    }
#endif // _DEBUG

    return exit_code;
}

/**
* @brief
*     Customize the console title and enable virtual terminal processing.
*/
void scan::setup_console()
{
    const int rcode{util::enable_vt_processing()};

    if (rcode != RCODE_NO_ERROR)
    {
        util::warnf("Virtual terminal processing is disabled: '%'", rcode);
    }
    else  // Set the console title
    {
        util::console_title(ArgParser::app_title());
    }
}

/**
* @brief
*     Perform the service scan against the specified target.
*/
int scan::run_scan(const Args& t_args)
{
    io_context ioc;
    int rcode{RCODE_ERROR};

    unique_ptr<TcpScanner> scannerp;
    shared_ptr<Args> argsp{std::make_shared<Args>(t_args)};

    // Use SSL/TLS capable TCP scanner
    if (t_args.tls_enabled)
    {
        scannerp = std::make_unique<TlsScanner>(ioc, argsp);
    }
    else  // Use standard TCP scanner
    {
        scannerp = std::make_unique<TcpScanner>(ioc, argsp);
    }

    try  // Run the network scan
    {
        scannerp->scan();
        rcode = RCODE_NO_ERROR;
    }
    catch (const Exception& ex)
    {
        ex.show();
    }
    return rcode;
}
