/*
* @file
*     thread_concepts.h
* @brief
*     Header file for multithreading concept constraints.
*/
#pragma once

#ifndef SCAN_THREAD_CONCEPTS_H
#define SCAN_THREAD_CONCEPTS_H

#include <functional>
#include <utility>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include "../threading/thread_aliases.h"
#include "../utils/aliases.h"
#include "concepts.h"

namespace scan
{
    /**
    * @brief
    *     Require that the given type is an invocable type that can
    *     be submitted to a thread pool for asynchronous execution.
    */
    template<class F>
    concept Postable = requires(thread_pool_t& r_pool, F&& r_func)
    {
        std::invoke<F>(std::forward<F>(r_func));
        asio::post(r_pool, std::forward<F>(r_func));
    };

    /**
    * @brief
    *     Require that the given type is an invocable type
    *     that does not return an object when it is called.
    */
    template<class F>
    concept Task = Postable<F> && Same<invoke_result_t<F>, void>;
}

#endif // !SCAN_THREAD_CONCEPTS_H
