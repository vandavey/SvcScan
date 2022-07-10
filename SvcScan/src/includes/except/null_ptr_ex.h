/*
*  null_ptr_ex.h
*  -------------
*  Header file for a null pointer argument exception
*/
#pragma once

#ifndef NULL_PTR_EX_H
#define NULL_PTR_EX_H

#include "null_arg_ex.h"

namespace scan
{
    /**
    * @brief  Null pointer argument exception.
    */
    class NullPtrEx final : public NullArgEx
    {
    private:  /* Type Aliases */
        using base_t = NullArgEx;

    private:  /* Constants */
        static constexpr char NAME[] = "scan::NullPtrEx";  // Exception name

    public:  /* Constructors & Destructor */
        NullPtrEx() = delete;
        NullPtrEx(const NullPtrEx &) = delete;
        NullPtrEx(NullPtrEx &&) = delete;
        NullPtrEx(const char *t_argp);
        NullPtrEx(const vector<string> &t_vect);

        virtual ~NullPtrEx() = default;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &t_os, const NullPtrEx &t_ex);

    public:  /* Operators */
        NullPtrEx &operator=(const NullPtrEx &) = default;
        NullPtrEx &operator=(NullPtrEx &&) = default;

    public:  /* Methods */
        virtual void show() const override;
        virtual string name() const noexcept override;

    private:  /* Methods */
        string init_msg() const noexcept;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline std::ostream &operator<<(std::ostream &t_os, const NullPtrEx &t_ex)
    {
        return t_os << static_cast<std::string>(t_ex);
    }
}

#endif // !NULL_PTR_EX_H
