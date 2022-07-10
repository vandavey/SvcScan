/*
*  exception.h
*  -----------
*  Header file for an abstract user-defined exception
*/
#pragma once

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "../contracts/i_string_castable.h"

namespace scan
{
    /**
    * @brief  Abstract user-defined exception.
    */
    class Exception : public IStringCastable
    {
    protected:  /* Type Aliases */
        using string = std::string;

    public:  /* Constructors & Destructor */
        Exception() = default;
        Exception(const Exception &) = default;
        Exception(Exception &&) = default;

        virtual ~Exception() = default;

    public:  /* Operators */
        Exception &operator=(const Exception &) = default;
        Exception &operator=(Exception &&) = default;

    public:  /* Methods */
        virtual void show() const = 0;

        virtual string name() const noexcept = 0;
    };
}

#endif // !EXCEPTION_H
