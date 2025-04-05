/*
* @file
*     thread_pool.h
* @brief
*     Header file for a thread pool.
*/
#pragma once

#ifndef SCAN_THREAD_POOL_H
#define SCAN_THREAD_POOL_H

#include <utility>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include "../concepts/thread_concepts.h"
#include "../ranges/algo.h"
#include "../utils/literals.h"
#include "thread_aliases.h"

namespace scan
{
    /**
    * @brief
    *     Execution thread pool.
    */
    class ThreadPool
    {
    private:  /* Fields */
        static const size_t m_cpu_threads;  // CPU thread count

        atomic_bool m_stopped;              // Thread pool execution stopped
        thread_pool m_pool;                 // Execution thread pool

    public:  /* Constructors & Destructor */
        ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool(size_t t_threads);

        virtual ~ThreadPool() = default;

    public:  /* Operators */
        ThreadPool& operator=(const ThreadPool&) = default;
        ThreadPool& operator=(ThreadPool&&) = default;

    public:  /* Methods */
        template<Task F>
        void post(F&& t_task);

        void stop();
        void wait();

        bool is_stopped() const noexcept;

    private:  /* Methods */
        /**
        * @brief
        *     Get the number of worker threads to use in thread pool initialization.
        */
        static constexpr size_t thread_count(size_t t_threads = 0_sz) noexcept
        {
            size_t threads{(algo::min)(m_cpu_threads, 16_sz)};

            if (t_threads > 0 && t_threads <= 32)
            {
                threads = t_threads;
            }
            return threads;
        }
    };
}

/**
* @brief
*     Submit a void task for execution by the underlying thread pool.
*/
template<scan::Task F>
inline void scan::ThreadPool::post(F&& t_task)
{
    asio::post(m_pool, std::forward<F>(t_task));
}

#endif // !SCAN_THREAD_POOL_H
