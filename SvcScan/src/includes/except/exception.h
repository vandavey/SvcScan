/*
*  exception.h
*  -----------
*  Source file for an abstract custom exception
*/
#pragma once

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "../contracts/istringcastable.h"

namespace scan
{
    /// ***
    /// Abstract base class for a custom exception
    /// ***
    class Exception : public IStringCastable
    {
    protected:  /* Type Aliases */
        using string = std::string;

    public:  /* Constructors & Destructor */
        Exception() = default;
        Exception(const Exception &) = delete;

        virtual ~Exception() = default;

    public:  /* Methods */
        virtual void show() const = 0;

        virtual string name() const noexcept = 0;
    };
}

#endif // !EXCEPTION_H
