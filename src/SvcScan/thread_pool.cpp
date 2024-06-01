/*
* @file
*     thread_pool.cpp
* @brief
*     Source file for a thread pool.
*/
#include "includes/threading/thread_pool.h"

/**
* @brief
*     Initialize the object.
*/
scan::ThreadPool::ThreadPool() : this_t(thread::hardware_concurrency())
{
}

/**
* @brief
*     Initialize the object.
*/
scan::ThreadPool::ThreadPool(const size_t &t_threads) : m_pool(t_threads)
{
    m_stopped = false;
    m_threads = t_threads > 0 ? t_threads : 1;
}

/**
* @brief
*     Stop the underlying worker threads from executing any further tasks.
*/
void scan::ThreadPool::stop()
{
    m_stopped = true;
    m_pool.stop();
}

/**
* @brief
*     Wait for all worker threads in the underlying thread pool to complete
*     their current tasks. Blocks until there is no more outstanding work.
*/
void scan::ThreadPool::wait()
{
    m_pool.wait();
}

/**
* @brief
*     Determine whether the underlying thread pool is no longer executing tasks.
*/
bool scan::ThreadPool::is_stopped() const noexcept
{
    return m_stopped;
}

/**
* @brief
*     Get the total number of worker threads in the underlying thread pool.
*/
size_t scan::ThreadPool::size() const noexcept
{
    return m_threads;
}
