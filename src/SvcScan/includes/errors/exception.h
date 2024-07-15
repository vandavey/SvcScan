/*
* @file
*     exception.h
* @brief
*     Header file for an abstract user-defined exception.
*/
#pragma once

#ifndef SCAN_EXCEPTION_H
#define SCAN_EXCEPTION_H

#include <exception>
#include <memory>
#include "../concepts/concepts.h"
#include "../contracts/i_string_castable.h"
#include "../utils/algo.h"
#include "../utils/aliases.h"
#include "error_const_defs.h"

namespace scan
{
    /**
    * @brief
    *     Abstract user-defined exception.
    */
    class Exception : public std::exception, public IStringCastable
    {
    private:  /* Type Aliases */
        using base_t = std::exception;

    public:  /* Fields */
        string msg;  // Error message

    public:  /* Constructors & Destructor */
        Exception() = default;
        Exception(const Exception &) = default;
        Exception(Exception &&) = default;
        Exception(const string &t_msg) noexcept;

        virtual ~Exception() = default;

    public:  /* Operators */
        Exception &operator=(const Exception &) = default;
        Exception &operator=(Exception &&) = default;

        friend ostream &operator<<(ostream &t_os, const Exception &t_ex);

    public:  /* Methods */
        /**
        * @brief
        *     Get a description of the exception.
        */
        constexpr const char *what() const noexcept override
        {
            return &msg[0];
        }

        virtual constexpr string name() const noexcept = 0;

        virtual void show() const = 0;

    protected:  /* Methods */
        /**
        * @brief
        *     Get the exception details as a string.
        */
        template<class T>
        constexpr string details(const string &t_key, const T &t_value) const
        {
            return details(map<string, string>{ pair{ t_key, t_value } });
        }

        /**
        * @brief
        *     Get the exception details as a string.
        */
        template<StringMap M = map<string, string>>
        constexpr string details(const M &t_map) const
        {
            const map<string, string> details_map
            {
                { EXCEPTION_KEY, name() },
                { INFORMATION_KEY, msg }
            };

            const size_t max_key_size{ algo::max_key_size(t_map, details_map) };
            const string header{ algo::fstr("----[ % ]----", ERROR_MSG_HEADER) };

            string_vector lines{ header };

            // Include common (base) error details
            for (const StringPair auto &pair : algo::pad_keys(details_map, max_key_size))
            {
                lines.push_back(algo::fstr(" % : %", pair.first, pair.second));
            }

            // Include custom (derived) error details
            for (const StringPair auto &pair : algo::pad_keys(t_map, max_key_size))
            {
                lines.push_back(algo::fstr(" % : %", pair.first, pair.second));
            }
            lines.push_back(algo::underline(header.size()));

            return algo::join_lines(lines);
        }
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const Exception &t_ex)
    {
        return t_os << static_cast<string>(t_ex);
    }
}

#endif // !SCAN_EXCEPTION_H
