/*
*  main.h
*  ------
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

    int run_scan(const Args &t_args);
}

int main(int argc, char *argv[]);

#endif // !SVCSCAN_H
