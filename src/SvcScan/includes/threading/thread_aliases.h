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
#include "../utils/aliases.h"

namespace scan
{
    using atomic_bool   = std::atomic_bool;
    using atomic_size_t = std::atomic_size_t;
    using mutex         = std::mutex;
    using thread        = std::thread;

    template<class T>
    using atomic_shared_ptr = std::atomic<shared_ptr<T>>;
}

#endif // !SCAN_THREAD_ALIASES_H
