/*
* @file
*     args.cpp
* @brief
*     Source file for parsed command-line arguments.
*/
#include <string>
#include <thread>
#include "includes/inet/net_const_defs.h"
#include "includes/threading/thread_aliases.h"
#include "includes/utils/args.h"

/**
* @brief
*     Initialize the object.
*/
scan::Args::Args() noexcept
{
    curl = out_json = tls_enabled = verbose = false;
    threads = thread::hardware_concurrency();

    timeout = CONN_TIMEOUT;
    uri = URI_ROOT;
}
