/*
* @file
*     null_ptr_ex.h
* @brief
*     Header file for a null pointer argument exception.
*/
#pragma once

#ifndef NULL_PTR_EX_H
#define NULL_PTR_EX_H

#include "null_arg_ex.h"

namespace scan
{
    /**
    * @brief
    *     Null pointer argument exception.
    */
    class NullPtrEx final : public NullArgEx
    {
    private:  /* Type Aliases */
        using base_t = NullArgEx;

    private:  /* Constants */
        static constexpr cstr_t NAME = "scan::NullPtrEx";  // Exception name

    public:  /* Constructors & Destructor */
        NullPtrEx() = delete;
        NullPtrEx(const NullPtrEx &t_ex);
        NullPtrEx(NullPtrEx &&) = delete;
        NullPtrEx(const char *t_argp);
        NullPtrEx(const string_vector &t_vect);

        virtual ~NullPtrEx() = default;

    public:  /* Operators */
        friend ostream &operator<<(ostream &t_os, const NullPtrEx &t_ex);

    public:  /* Operators */
        NullPtrEx &operator=(const NullPtrEx &) = default;
        NullPtrEx &operator=(NullPtrEx &&) = default;

    public:  /* Methods */
        void show() const override;

        string name() const noexcept override;

    private:  /* Methods */
        string init_msg() const noexcept;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const NullPtrEx &t_ex)
    {
        return t_os << static_cast<string>(t_ex);
    }
}

#endif // !NULL_PTR_EX_H
