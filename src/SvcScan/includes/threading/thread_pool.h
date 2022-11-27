/*
*  thread_pool.h
*  -------------
*  Header file for a thread pool
*/
#pragma once

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <future>
#include "../concepts/thread_concepts.h"
#include "../threading/thread_defs.h"
#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Execution thread pool.
    */
    class ThreadPool
    {
    private:  /* Type Aliases */
        using this_t = ThreadPool;

        using thread_pool = asio::thread_pool;

        template<class T>
        using invoke_future_t = std::future<invoke_result_t<T>>;

        template<class T>
        using invoke_promise_t = std::promise<invoke_result_t<T>>;

    private:  /* Fields */
        atomic_bool m_stopped;    // Thread pool execution stopped
        atomic_size_t m_threads;  // Worker thread count

        thread_pool m_pool;       // Execution thread pool

    public:  /* Constructors & Destructor */
        ThreadPool();
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;

        ThreadPool(const size_t &t_threads);

        virtual ~ThreadPool() = default;

    public:  /* Operators */
        ThreadPool &operator=(const ThreadPool &) = default;
        ThreadPool &operator=(ThreadPool &&) noexcept = default;

    public:  /* Methods */
        template<Task F>
        void post(F &&t_task);

        void stop();
        void wait();

        bool is_stopped() const noexcept;

        size_t size() const noexcept;

        template<ValueTask F>
        invoke_future_t<F> submit(F &&t_task);
    };
}

/**
* @brief  Submit a void task for execution by the underlying thread pool.
*/
template<scan::Task F>
inline void scan::ThreadPool::post(F &&t_task)
{
    asio::post(m_pool, std::forward<F>(t_task));
}

/**
* @brief  Submit a value task for execution by the underlying thread pool.
*/
template<scan::ValueTask F>
inline scan::ThreadPool::invoke_future_t<F> scan::ThreadPool::submit(F &&t_task)
{
    invoke_promise_t<F> promise;
    invoke_future_t<F> future = promise.get_future();

    asio::post(m_pool, [&promise, &t_task]() mutable -> void
    {
        promise.set_value(t_task());
    });
    return future;
}

#endif // !THREAD_POOL_H
