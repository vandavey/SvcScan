/*
* @file
*     runtime_ex.h
* @brief
*     Header file for a runtime exception.
*/
#pragma once

#ifndef SCAN_RUNTIME_EX_H
#define SCAN_RUNTIME_EX_H

#include "../utils/aliases.h"
#include "error_const_defs.h"
#include "exception.h"

namespace scan
{
    /**
    * @brief
    *     Runtime exception.
    */
    class RuntimeEx : public Exception
    {
    private:  /* Type Aliases */
        using base_t = Exception;

    public:  /* Fields */
        string caller;  // Exception origin location

    public:  /* Constructors & Destructor */
        RuntimeEx() = delete;
        RuntimeEx(const RuntimeEx&) = default;
        RuntimeEx(RuntimeEx&&) = default;
        RuntimeEx(const string& t_caller, const string& t_msg);

        virtual ~RuntimeEx() = default;

    public:  /* Operators */
        RuntimeEx& operator=(const RuntimeEx&) = default;
        RuntimeEx& operator=(RuntimeEx&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const override
        {
            return details(LOCATION_KEY, caller);
        }

    public:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return RUNTIME_EX_NAME;
        }

        virtual void show() const override;
    };
}

#endif // !SCAN_RUNTIME_EX_H
