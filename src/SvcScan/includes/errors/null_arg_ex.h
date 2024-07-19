/*
* @file
*     null_arg_ex.h
* @brief
*     Header file for a null argument exception.
*/
#pragma once

#ifndef SCAN_NULL_ARG_EX_H
#define SCAN_NULL_ARG_EX_H

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
    private:  /* Type Aliases */
        using base_t = ArgEx;

    public:  /* Constructors & Destructor */
        NullArgEx() = delete;
        NullArgEx(const NullArgEx&) = default;
        NullArgEx(NullArgEx&&) = default;
        NullArgEx(const char* t_argp);
        NullArgEx(const string_vector& t_vect);

        virtual ~NullArgEx() = default;

    protected:  /* Constructors */
        NullArgEx(const string_vector& t_vect, const string& t_msg);

    public:  /* Operators */
        NullArgEx& operator=(const NullArgEx&) = default;
        NullArgEx& operator=(NullArgEx&&) = default;

    public:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return NULL_ARG_EX_NAME;
        }

        virtual void show() const override;

    private:  /* Methods */
        /**
        * @brief
        *     Get a description of the exception.
        */
        virtual constexpr string init_msg() const noexcept
        {
            return NULL_ARG_EX_MSG;
        }
    };
}

#endif // !SCAN_NULL_ARG_EX_H
