/*
*  exception.h
*  -----------
*  Source file for an abstract custom exception
*/
#pragma once

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

namespace scan
{
    /// ***
    /// Abstract base class for a custom exception
    /// ***
    class Exception
    {
    protected:  /* Type Aliases */
        using string = std::string;

    public:  /* Constructors & Destructor */
        Exception() = default;

        virtual ~Exception() = default;

    private:  /* Constructors (deleted) */
        Exception(const Exception &) = delete;

    public:  /* Operators */
        virtual operator string() const = 0;

    public:  /* Methods */
        virtual void show() const = 0;

        virtual string name() const noexcept = 0;
    };
}

#endif // !EXCEPTION_H
