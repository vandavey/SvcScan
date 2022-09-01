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

#include <memory>
#include <boost/asio/io_context.hpp>
#include "utils/algorithm.h"
#include "utils/args.h"

namespace scan
{
    namespace
    {
        using algo       = Algorithm;
        using io_context = boost::asio::io_context;

        template<class T>
        using shared_ptr = std::shared_ptr<T>;

        template<class T>
        using unique_ptr = std::unique_ptr<T>;
    }

    void setup_console();

    int run_scan(io_context &t_ioc, const Args &t_args);
}

#endif // !SVCSCAN_H
