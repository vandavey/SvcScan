/*
* @file
*     logic_ex.h
* @brief
*     Header file for a logic exception.
*/
#pragma once

#ifndef SCAN_LOGIC_EX_H
#define SCAN_LOGIC_EX_h

#include <string>
#include "../utils/aliases.h"
#include "error_aliases.h"
#include "error_const_defs.h"
#include "exception.h"

namespace scan
{
    /**
    * @brief
    *     Logic exception.
    */
    class LogicEx : public Exception, public logic_error
    {
    private:  /* Fields */
        string m_caller;  // Exception origin location

    public:  /* Constructors & Destructor */
        LogicEx() = delete;
        LogicEx(const LogicEx&) = default;
        LogicEx(LogicEx&&) = default;
        LogicEx(const string& t_msg, const string& t_caller);

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

    protected:  /* Methods */
        /**
        * @brief
        *     Get the exception name.
        */
        virtual constexpr string name() const noexcept override
        {
            return static_cast<string>(LOGIC_EX_NAME);
        }
    };
}

#endif // !SCAN_LOGIC_EX_H
