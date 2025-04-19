/*
* @file
*     exception.h
* @brief
*     Header file for a virtual user-defined exception.
*/
#pragma once

#ifndef SCAN_EXCEPTION_H
#define SCAN_EXCEPTION_H

#include <string>
#include <vector>
#include "../concepts/concepts.h"
#include "../contracts/i_string_castable.h"
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "error_const_defs.h"

namespace scan
{
    /**
    * @brief
    *     Virtual user-defined exception.
    */
    class Exception : public IStringCastable
    {
    protected:  /* Fields */
        string m_msg;  // Error message

    public:  /* Constructors & Destructor */
        Exception() = default;
        Exception(const Exception&) = default;
        Exception(Exception&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Exception(const string& t_msg) noexcept
        {
            m_msg = t_msg;
        }

        virtual ~Exception() = default;

    public:  /* Operators */
        Exception& operator=(const Exception&) = default;
        Exception& operator=(Exception&&) = default;

        virtual constexpr operator string() const override = 0;

        friend ostream& operator<<(ostream& t_os, const Exception& t_ex);

    protected:  /* Methods */
        /**
        * @brief
        *     Get the exception details as a string.
        */
        template<class T>
        constexpr string details(const string& t_key, const T& t_value) const
        {
            return details(map<string, T>{pair{t_key, t_value}});
        }

        /**
        * @brief
        *     Get the exception details as a string.
        */
        constexpr string details(const StringMap auto& t_map) const
        {
            const map<string, string> details_map
            {
                {EXCEPTION_KEY, name()},
                {INFORMATION_KEY, m_msg}
            };

            const size_t max_key_size{algo::max_key_size(t_map, details_map)};
            vector<string> lines{algo::fstr("----[ % ]----", ERROR_MSG_HEADER)};

            // Include common (base) error details
            for (const StringPair auto& pair : algo::pad_keys(details_map, max_key_size))
            {
                lines.emplace_back(algo::fstr(" % : %", pair.first, pair.second));
            }

            // Include custom (derived) error details
            for (const StringPair auto& pair : algo::pad_keys(t_map, max_key_size))
            {
                lines.emplace_back(algo::fstr(" % : %", pair.first, pair.second));
            }
            lines.emplace_back(algo::underline(lines[0].size()));

            return algo::join_lines(lines);
        }

        virtual constexpr string name() const noexcept = 0;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream& operator<<(ostream& t_os, const Exception& t_ex)
    {
        return t_os << static_cast<string>(t_ex);
    }
}

#endif // !SCAN_EXCEPTION_H
