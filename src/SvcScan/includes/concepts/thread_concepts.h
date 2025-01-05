/*
* @file
*     thread_concepts.h
* @brief
*     Header file for multithreading concept constraints.
*/
#pragma once

#ifndef SCAN_THREAD_CONCEPTS_H
#define SCAN_THREAD_CONCEPTS_H

#include <concepts>
#include <functional>
#include <utility>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
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
    concept Postable = requires(F&& r_func, asio::thread_pool r_pool)
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
    concept Task = Postable<F> && std::same_as<invoke_result_t<F>, void>;

    /**
    * @brief
    *     Require that the given type is an invocable
    *     type that returns an object when it is called.
    */
    template<class F>
    concept ValueTask = Postable<F> && NotSameAs<invoke_result_t<F>, void>;
}

#endif // !SCAN_THREAD_CONCEPTS_H
