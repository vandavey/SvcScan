/*
* @file
*     logic_ex.h
* @brief
*     Header file for a logic exception.
*/
#pragma once

#ifndef SCAN_LOGIC_EX_H
#define SCAN_LOGIC_EX_h

#include "../utils/aliases.h"
#include "error_const_defs.h"
#include "exception.h"

namespace scan
{
    /**
    * @brief
    *     Logic exception.
    */
    class LogicEx : public Exception
    {
    private:  /* Type Aliases */
        using base_t = Exception;

    public:  /* Fields */
        string caller;  // Exception origin location

    public:  /* Constructors & Destructor */
        LogicEx() = delete;
        LogicEx(const LogicEx&) = default;
        LogicEx(LogicEx&&) = default;
        LogicEx(const string& t_caller, const string& t_msg);

        virtual ~LogicEx() = default;

    public:  /* Operators */
        LogicEx& operator=(const LogicEx&) = default;
        LogicEx& operator=(LogicEx&&) = default;

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
            return LOGIC_EX_NAME;
        }

        virtual void show() const override;
    };
}

#endif // !SCAN_LOGIC_EX_H
