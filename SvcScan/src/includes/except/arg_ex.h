/*
*  arg_ex.h
*  --------
*  Header file for an invalid argument exception
*/
#pragma once

#ifndef ARG_EX_H
#define ARG_EX_H

#include <iostream>
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
        template<class T>
        using vector = std::vector<T>;

    private:  /* Type Aliases */
        using base_t = std::invalid_argument;

    private:  /* Constants */
        static constexpr char NAME[] = "scan::ArgEx";

    public:  /* Fields */
        string arg;  // Invalid argument
        string msg;  // Error message

    public:  /* Constructors & Destructor */
        ArgEx() = delete;
        ArgEx(const ArgEx &) = delete;
        ArgEx(const char *t_argp, const string &t_msg);
        ArgEx(const vector<string> &t_vect, const string &t_msg);

        virtual ~ArgEx() = default;

    public:  /* Operators */
        operator string() const override;

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
