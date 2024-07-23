/*
* @file
*     arg_ex.h
* @brief
*     Header file for an invalid argument exception.
*/
#pragma once

#ifndef SCAN_ARG_EX_H
#define SCAN_ARG_EX_H

#include "../utils/aliases.h"
#include "error_const_defs.h"
#include "exception.h"

namespace scan
{
    /**
    * @brief
    *     Invalid argument exception.
    */
    class ArgEx : public Exception
    {
    private:  /* Type Aliases */
        using base_t = Exception;

    public:  /* Fields */
        string arg;  // Invalid argument

    public:  /* Constructors & Destructor */
        ArgEx() = delete;
        ArgEx(const ArgEx&) = default;
        ArgEx(ArgEx&&) = default;
        ArgEx(const char* t_argp, const string& t_msg);
        ArgEx(const string_vector& t_vect, const string& t_msg);

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
            return details(ARGUMENTS_KEY, arg);
        }

    public:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return ARG_EX_NAME;
        }

        virtual void show() const override;
    };
}

#endif // !SCAN_ARG_EX_H
