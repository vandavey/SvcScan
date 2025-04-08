/*
* @file
*     thread_aliases.h
* @brief
*     Header file for multithreading type aliases.
*/
#pragma once

#ifndef SCAN_THREAD_ALIASES_H
#define SCAN_THREAD_ALIASES_H

#include <atomic>
#include <mutex>
#include <thread>
#include <boost/asio/thread_pool.hpp>
#include "../utils/aliases.h"

namespace scan
{
    using atomic_bool   = std::atomic_bool;
    using mutex         = std::mutex;
    using thread        = std::thread;
    using thread_pool_t = asio::thread_pool;

    template<class T>
    using atomic_ptr_t = std::atomic<shared_ptr<T>>;

    template<class... ArgsT>
    using scoped_lock = std::scoped_lock<ArgsT...>;
}

#endif // !SCAN_THREAD_ALIASES_H
