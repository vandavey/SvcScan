/*
* @file
*     thread_pool.cpp
* @brief
*     Source file for a thread pool.
*/
#include <thread>
#include "includes/threading/thread_pool.h"

/**
* @brief
*     Maximum concurrent CPU thread count.
*/
const size_t scan::ThreadPool::m_cpu_threads{ thread::hardware_concurrency() };

/**
* @brief
*     Initialize the object.
*/
scan::ThreadPool::ThreadPool() : ThreadPool{ 0_st }
{
}

/**
* @brief
*     Initialize the object.
*/
scan::ThreadPool::ThreadPool(const size_t &t_threads) : m_pool{ thread_count(t_threads) }
{
    m_stopped = false;
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
    return m_stopped.load();
}
