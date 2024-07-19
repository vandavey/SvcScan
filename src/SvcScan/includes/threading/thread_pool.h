/*
* @file
*     thread_pool.h
* @brief
*     Header file for a thread pool.
*/
#pragma once

#ifndef SCAN_THREAD_POOL_H
#define SCAN_THREAD_POOL_H

#include <future>
#include <utility>
#include <sdkddkver.h>
#include <boost/asio/thread_pool.hpp>
#include "../concepts/thread_concepts.h"
#include "../utils/aliases.h"
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
    private:  /* Type Aliases */
        using thread_pool = asio::thread_pool;

        template<class T>
        using invoke_future_t = std::future<invoke_result_t<T>>;

        template<class T>
        using invoke_promise_t = std::promise<invoke_result_t<T>>;

    private:  /* Fields */
        static const size_t m_cpu_threads;  // CPU thread count

        atomic_bool m_stopped;              // Thread pool execution stopped
        thread_pool m_pool;                 // Execution thread pool

    public:  /* Constructors & Destructor */
        ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool(const size_t& t_threads);

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

        template<ValueTask F>
        invoke_future_t<F> submit(F&& t_task);

    private:  /* Methods */
        /**
        * @brief
        *     Get the number of worker threads to use in thread pool initialization.
        */
        static constexpr size_t thread_count(const size_t& t_threads = 0_st) noexcept
        {
            size_t threads{ m_cpu_threads <= 16 ? m_cpu_threads : 16_st };

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

/**
* @brief
*     Submit a value task for execution by the underlying thread pool.
*/
template<scan::ValueTask F>
inline scan::ThreadPool::invoke_future_t<F> scan::ThreadPool::submit(F&& t_task)
{
    invoke_promise_t<F> promise;
    invoke_future_t<F> future = promise.get_future();

    asio::post(m_pool, [&promise, &t_task]() mutable -> void
    {
        promise.set_value(t_task());
    });
    return future;
}

#endif // !SCAN_THREAD_POOL_H
