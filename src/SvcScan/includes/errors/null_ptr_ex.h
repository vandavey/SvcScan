/*
* @file
*     null_ptr_ex.h
* @brief
*     Header file for a null pointer argument exception.
*/
#pragma once

#ifndef SCAN_NULL_PTR_EX_H
#define SCAN_NULL_PTR_EX_H

#include <string>
#include <utility>
#include "../concepts/concepts.h"
#include "../utils/aliases.h"
#include "error_const_defs.h"
#include "null_arg_ex.h"

namespace scan
{
    /**
    * @brief
    *     Null pointer argument exception.
    */
    class NullPtrEx : public NullArgEx
    {
    public:  /* Constructors & Destructor */
        NullPtrEx() = delete;
        NullPtrEx(const NullPtrEx&) = default;
        NullPtrEx(NullPtrEx&&) = default;

        template<StringLike... ArgsT>
            requires AtLeastOne<ArgsT...>
        NullPtrEx(ArgsT&&... t_args);

        virtual ~NullPtrEx() = default;

    public:  /* Operators */
        NullPtrEx& operator=(const NullPtrEx&) = default;
        NullPtrEx& operator=(NullPtrEx&&) = default;

    protected:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return static_cast<string>(NULL_PTR_EX_NAME);
        }
    };
}

/**
* @brief
*     Initialize the object.
*/
template<scan::StringLike... ArgsT>
    requires scan::AtLeastOne<ArgsT...>
inline scan::NullPtrEx::NullPtrEx(ArgsT&&... t_args)
    : NullArgEx{true, NULL_PTR_EX_MSG, std::forward<ArgsT>(t_args)...}
{
}

#endif // !SCAN_NULL_PTR_EX_H
