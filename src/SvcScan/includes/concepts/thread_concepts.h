/*
*  thread_concepts.h
*  -----------------
*  Header file containing multithreading concept constraints
*/
#pragma once

#ifndef THREAD_CONCEPTS_H
#define THREAD_CONCEPTS_H

#include <sdkddkver.h>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include "../utils/type_defs.h"
#include "type_concepts.h"

namespace scan
{
    /**
    * @brief  Require that the given type is an invocable type that can be
    *         submitted to a thread pool for asynchronous execution.
    */
    template<class F>
    concept Postable = requires(F &&t_func, asio::thread_pool t_pool)
    {
        { std::invoke<F>(std::forward<F>(t_func)) };
        { asio::post(t_pool, std::forward<F>(t_func)) };
    };

    /**
    * @brief  Require that the given type is an invocable type that
    *         does not return an object when it is called.
    */
    template<class F>
    concept Task = Postable<F> && std::same_as<invoke_result_t<F>, void>;

    /**
    * @brief  Require that the given type is an invocable type that
    *         returns an object when it is called.
    */
    template<class F>
    concept ValueTask = Postable<F> && NotSameAs<invoke_result_t<F>, void>;
}

#endif // !THREAD_CONCEPTS_H
