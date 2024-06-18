/*
* @file
*     thread_alias.h
* @brief
*     Header file for multithreading type aliases.
*/
#pragma once

#ifndef SCAN_THREAD_ALIAS_H
#define SCAN_THREAD_ALIAS_H

#include <atomic>
#include <mutex>
#include <thread>
#include "../utils/alias.h"

namespace scan
{
    using atomic_bool   = std::atomic_bool;
    using atomic_size_t = std::atomic_size_t;
    using mutex         = std::mutex;
    using thread        = std::thread;

    template<class T>
    using atomic_ptr = std::atomic<shared_ptr<T>>;
}

#endif // !SCAN_THREAD_ALIAS_H
