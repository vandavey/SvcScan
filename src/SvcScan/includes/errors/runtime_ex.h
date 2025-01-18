/*
* @file
*     runtime_ex.h
* @brief
*     Header file for a runtime exception.
*/
#pragma once

#ifndef SCAN_RUNTIME_EX_H
#define SCAN_RUNTIME_EX_H

#include <string>
#include "../utils/aliases.h"
#include "error_aliases.h"
#include "error_const_defs.h"
#include "exception.h"

namespace scan
{
    /**
    * @brief
    *     Runtime exception.
    */
    class RuntimeEx : public Exception, public runtime_error
    {
    private:  /* Fields */
        string m_caller;  // Exception origin location

    public:  /* Constructors & Destructor */
        RuntimeEx() = delete;
        RuntimeEx(const RuntimeEx&) = default;
        RuntimeEx(RuntimeEx&&) = default;
        RuntimeEx(const string& t_msg, const string& t_caller);

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
            return details(LOCATION_KEY, m_caller);
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

        virtual void show() const override;

    protected:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return static_cast<string>(RUNTIME_EX_NAME);
        }
    };
}

#endif // !SCAN_RUNTIME_EX_H
