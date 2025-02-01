/*
* @file
*     null_arg_ex.h
* @brief
*     Header file for a null argument exception.
*/
#pragma once

#ifndef SCAN_NULL_ARG_EX_H
#define SCAN_NULL_ARG_EX_H

#include <string>
#include <utility>
#include "../concepts/concepts.h"
#include "../utils/aliases.h"
#include "arg_ex.h"
#include "error_const_defs.h"

namespace scan
{
    /**
    * @brief
    *     Null argument exception.
    */
    class NullArgEx : public ArgEx
    {
    public:  /* Constructors & Destructor */
        NullArgEx() = delete;
        NullArgEx(const NullArgEx&) = default;
        NullArgEx(NullArgEx&&) = default;

        template<String... ArgsT>
            requires AtLeastOne<ArgsT...>
        NullArgEx(const ArgsT&... t_args);

        template<String... ArgsT>
            requires AtLeastOne<ArgsT...>
        NullArgEx(ArgsT&&... t_args);

        virtual ~NullArgEx() = default;

    protected:  /* Constructors */
        template<String... ArgsT>
            requires AtLeastOne<ArgsT...>
        explicit NullArgEx(bool, const string& t_msg, const ArgsT&... t_args);

        template<String... ArgsT>
            requires AtLeastOne<ArgsT...>
        explicit NullArgEx(bool, const string& t_msg, ArgsT&&... t_args);

    public:  /* Operators */
        NullArgEx& operator=(const NullArgEx&) = default;
        NullArgEx& operator=(NullArgEx&&) = default;

    protected:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return static_cast<string>(NULL_ARG_EX_NAME);
        }
    };
}

/**
* @brief
*     Initialize the object.
*/
template<scan::String... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline scan::NullArgEx::NullArgEx(const ArgsT&... t_args)
    : ArgEx{NULL_ARG_EX_MSG, t_args...}
{
}

/**
* @brief
*     Initialize the object.
*/
template<scan::String... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline scan::NullArgEx::NullArgEx(ArgsT&&... t_args)
    : ArgEx{NULL_ARG_EX_MSG, std::forward<ArgsT>(t_args)...}
{
}

/**
* @brief
*     Initialize the object.
*/
template<scan::String... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline scan::NullArgEx::NullArgEx(bool, const string& t_msg, const ArgsT&... t_args)
    : ArgEx{t_msg, t_args...}
{
}

/**
* @brief
*     Initialize the object.
*/
template<scan::String... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline scan::NullArgEx::NullArgEx(bool, const string& t_msg, ArgsT&&... t_args)
    : ArgEx{t_msg, std::forward<ArgsT>(t_args)...}
{
}

#endif // !SCAN_NULL_ARG_EX_H
