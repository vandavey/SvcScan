/*
*  runtimeex.h
*  -----------
*  Header file for a runtime exception
*/
#pragma once

#ifndef RUNTIME_EX_H
#define RUNTIME_EX_H

#include <iostream>
#include <stdexcept>
#include "exception.h"

namespace scan
{
    /// ***
    /// Unexpected runtime exception
    /// ***
    class RuntimeEx : public Exception, public std::runtime_error
    {
    private:  /* Type Aliases */
        using base = std::runtime_error;

    private: /* Constants */
        static constexpr char NAME[] = "scan::RuntimeEx";

    public:  /* Fields */
        string caller;  // Method that threw exception
        string msg;     // Error message

    public:  /* Constructors & Destructor */
        RuntimeEx(const string &t_caller, const string &t_msg);

        virtual ~RuntimeEx() = default;

    private:  /* Constructors (deleted) */
        RuntimeEx() = delete;
        RuntimeEx(const RuntimeEx &) = delete;

    public:  /* Operators */
        operator string() const;

        friend std::ostream &operator<<(std::ostream &t_os, const RuntimeEx &t_ex);

    public:  /* Methods */
        virtual void show() const;

        virtual string name() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &scan::operator<<(std::ostream &t_os, const RuntimeEx &t_ex)
    {
        return (t_os << static_cast<std::string>(t_ex));
    }
}

#endif // !RUNTIME_EX_H
