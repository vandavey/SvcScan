/*
* @file
*     arg_ex.h
* @brief
*     Header file for an invalid argument exception.
*/
#pragma once

#ifndef ARG_EX_H
#define ARG_EX_H

#include "exception.h"

namespace scan
{
    /**
    * @brief
    *     Invalid argument exception.
    */
    class ArgEx : public Exception
    {
    private:  /* Type Aliases */
        using base_t = Exception;

    private:  /* Constants */
        static constexpr cstr_t NAME = "scan::ArgEx";  // Exception name

    public:  /* Fields */
        string arg;  // Invalid argument

    public:  /* Constructors & Destructor */
        ArgEx() = delete;
        ArgEx(const ArgEx &t_ex);
        ArgEx(ArgEx &&) = delete;
        ArgEx(const char *t_argp, const string &t_msg);
        ArgEx(const string_vector &t_vect, const string &t_msg);

        virtual ~ArgEx() = default;

    public:  /* Operators */
        ArgEx &operator=(const ArgEx &) = default;
        ArgEx &operator=(ArgEx &&) = default;

        operator std::string() const override;

        friend ostream &operator<<(ostream &t_os, const ArgEx &t_ex);

    public:  /* Methods */
        virtual void show() const;

        virtual string name() const noexcept;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const ArgEx &t_ex)
    {
        return t_os << static_cast<string>(t_ex);
    }
}

#endif // !ARG_EX_H
