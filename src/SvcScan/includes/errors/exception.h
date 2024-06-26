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
#include "../contracts/i_string_castable.h"
#include "../utils/alias.h"

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

    public:  /* Methods */
        virtual void show() const = 0;

        virtual const char *what() const noexcept override;
        virtual string name() const noexcept = 0;
    };
}

#endif // !SCAN_EXCEPTION_H
