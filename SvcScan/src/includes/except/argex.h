/*
*  argex.h
*  -------
*  Header file for invalid argument exceptions
*/
#pragma once

#ifndef ARG_EX_H
#define ARG_EX_H

#include <iostream>
#include <stdexcept>
#include <vector>
#include "exception.h"

namespace scan
{
    /// ***
    /// Standard argument exception
    /// ***
    class ArgEx : public Exception, public std::invalid_argument
    {
    protected:  /* Type Aliases */
        using vector_s = std::vector<string>;

    private:  /* Type Aliases */
        using base = std::invalid_argument;

    private:  /* Constants */
        static constexpr char NAME[] = "scan::ArgEx";

    public:  /* Fields */
        string arg;  // Invalid argument
        string msg;  // Error message

    public:  /* Constructors & Destructor */
        ArgEx(const char *t_argp, const string &t_msg);
        ArgEx(const vector_s &t_vect, const string &t_msg);

        virtual ~ArgEx() = default;

    private:  /* Constructors (deleted) */
        ArgEx() = delete;
        ArgEx(const ArgEx &) = delete;

    public:  /* Operators */
        operator string() const;

        friend std::ostream &operator<<(std::ostream &t_os, const ArgEx &t_ex);

    public:  /* Methods */
        virtual void show() const;

        virtual string name() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const ArgEx &t_ex)
    {
        return (t_os << static_cast<std::string>(t_ex));
    }
}

#endif // !ARG_EX_H
