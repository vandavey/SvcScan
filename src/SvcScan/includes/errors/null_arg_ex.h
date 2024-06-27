/*
* @file
*     null_arg_ex.h
* @brief
*     Header file for a null argument exception.
*/
#pragma once

#ifndef SCAN_NULL_ARG_EX_H
#define SCAN_NULL_ARG_EX_H

#include "../utils/alias.h"
#include "arg_ex.h"

namespace scan
{
    /**
    * @brief
    *     Null argument exception.
    */
    class NullArgEx : public ArgEx
    {
    private:  /* Type Aliases */
        using base_t = ArgEx;

    private:  /* Constants */
        static constexpr cstr_t NAME = "scan::NullArgEx";  // Exception name

    public:  /* Constructors & Destructor */
        NullArgEx() = delete;
        NullArgEx(const NullArgEx &t_ex) noexcept;
        NullArgEx(NullArgEx &&) = delete;
        NullArgEx(const char *t_argp);
        NullArgEx(const string_vector &t_vect);

        virtual ~NullArgEx() = default;

    protected:  /* Constructors */
        NullArgEx(const string_vector &t_vect, const string &t_msg);

    public:  /* Operators */
        NullArgEx &operator=(const NullArgEx &) = default;
        NullArgEx &operator=(NullArgEx &&) = default;

        friend ostream &operator<<(ostream &t_os, const NullArgEx &t_ex);

    public:  /* Methods */
        virtual void show() const override;

        virtual string name() const noexcept override;

    private:  /* Methods */
        string init_msg() const noexcept;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const NullArgEx &t_ex)
    {
        return t_os << static_cast<string>(t_ex);
    }
}

#endif // !SCAN_NULL_ARG_EX_H
