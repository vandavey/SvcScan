/*
* @file
*     null_ptr_ex.h
* @brief
*     Header file for a null pointer argument exception.
*/
#pragma once

#ifndef SCAN_NULL_PTR_EX_H
#define SCAN_NULL_PTR_EX_H

#include "../utils/aliases.h"
#include "error_const_defs.h"
#include "null_arg_ex.h"

namespace scan
{
    /**
    * @brief
    *     Null pointer argument exception.
    */
    class NullPtrEx final : public NullArgEx
    {
    private:  /* Type Aliases */
        using base_t = NullArgEx;

    public:  /* Constructors & Destructor */
        NullPtrEx() = delete;
        NullPtrEx(const NullPtrEx &) = default;
        NullPtrEx(NullPtrEx &&) = default;
        NullPtrEx(const char *t_argp);
        NullPtrEx(const string_vector &t_vect);

        virtual ~NullPtrEx() = default;

    public:  /* Operators */
        NullPtrEx &operator=(const NullPtrEx &) = default;
        NullPtrEx &operator=(NullPtrEx &&) = default;

    public:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        constexpr string name() const noexcept override
        {
            return NULL_PTR_EX_NAME;
        }

        void show() const override;

    private:  /* Methods */
        /**
        * @brief
        *     Get a description of the exception.
        */
        constexpr string init_msg() const noexcept override
        {
            return NULL_PTR_EX_MSG;
        }
    };
}

#endif // !SCAN_NULL_PTR_EX_H
