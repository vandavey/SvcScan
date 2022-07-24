/*
*  svcscan.h
*  ---------
*  Header file for the application entry point
*/
#pragma once

#ifndef SVCSCAN_H
#define SVCSCAN_H

#ifndef UNICODE
#  define UNICODE 1
#endif // !UNICODE

#include <boost/asio/io_context.hpp>
#include "inet/scanners/scanner.h"
#include "utils/args.h"

namespace
{
    using io_context  = boost::asio::io_context;
    using scanner_ptr = std::unique_ptr<scan::Scanner>;
}

namespace scan
{
    void setup_console();

    int run_scan(io_context &t_ioc, const Args &t_args);
}

#endif // !SVCSCAN_H
