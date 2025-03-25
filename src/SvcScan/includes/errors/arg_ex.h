/*
* @file
*     arg_ex.h
* @brief
*     Header file for an invalid argument exception.
*/
#pragma once

#ifndef SCAN_ARG_EX_H
#define SCAN_ARG_EX_H

#include <stdexcept>
#include <string>
#include <utility>
#include "../concepts/concepts.h"
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "error_aliases.h"
#include "error_const_defs.h"
#include "exception.h"

namespace scan
{
    /**
    * @brief
    *     Invalid argument exception.
    */
    class ArgEx : public Exception, public invalid_argument
    {
    private:  /* Fields */
        vector<string> m_args;  // Invalid arguments

    public:  /* Constructors & Destructor */
        ArgEx() = delete;
        ArgEx(const ArgEx&) = default;
        ArgEx(ArgEx&&) = default;

        template<StringLike... ArgsT>
            requires AtLeastOne<ArgsT...>
        ArgEx(const string& t_msg, const ArgsT&... t_args);

        template<StringLike... ArgsT>
            requires AtLeastOne<ArgsT...>
        ArgEx(const string& t_msg, ArgsT&&... t_args);

        virtual ~ArgEx() = default;

    public:  /* Operators */
        ArgEx& operator=(const ArgEx&) = default;
        ArgEx& operator=(ArgEx&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const override
        {
            return details(ARGUMENTS_KEY, algo::join(m_args, ", "));
        }

    public:  /* Methods */
        /**
        * @brief
        *     Get a description of the exception.
        */
        constexpr const char* what() const noexcept override
        {
            return &m_msg[0];
        }

    protected:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return static_cast<string>(ARG_EX_NAME);
        }
    };
}

/**
* @brief
*     Initialize the object.
*/
template<scan::StringLike... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline scan::ArgEx::ArgEx(const string& t_msg, const ArgsT&... t_args)
    : Exception{t_msg}, invalid_argument{t_msg}
{
    (m_args.push_back(t_args), ...);
}

/**
* @brief
*     Initialize the object.
*/
template<scan::StringLike... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline scan::ArgEx::ArgEx(const string& t_msg, ArgsT&&... t_args)
    : Exception{t_msg}, invalid_argument{t_msg}
{
    (m_args.push_back(std::move(t_args)), ...);
}

#endif // !SCAN_ARG_EX_H
