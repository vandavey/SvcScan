/*
* @file
*     thread_concepts.h
* @brief
*     Header file for multithreading concept constraints.
*/
#pragma once

#ifndef SCAN_THREAD_CONCEPTS_H
#define SCAN_THREAD_CONCEPTS_H

#include <utility>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include "../threading/thread_aliases.h"
#include "concepts.h"

namespace scan
{
    /**
    * @brief
    *     Require that a type is an invocable type that returns void and
    *     can be submitted to a thread pool for asynchronous execution.
    */
    template<class F>
    concept Task = NullaryVoid<F> && requires(thread_pool_t& r_pool, F&& r_task)
    {
        { asio::post(r_pool, std::forward<F>(r_task)) } -> Same<void>;
    };
}

#endif // !SCAN_THREAD_CONCEPTS_H
