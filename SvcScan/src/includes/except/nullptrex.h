/*
*  nullptrex.h
*  -----------
*  Header file for nullptr argument exceptions
*/
#pragma once

#ifndef NULL_PTR_EX_H
#define NULL_PTR_EX_H

#include "nullargex.h"

namespace scan
{
    /// ***
    /// Null pointer argument exception
    /// ***
    class NullPtrEx : public NullArgEx
    {
    private:  /* Types & Constants */
        using base = NullArgEx;

        static constexpr char NAME[] = "scan::NullPtrEx";

    public:  /* Constructors & Destructor */
        NullPtrEx(const NullPtrEx &) = default;
        explicit NullPtrEx(const il_s &t_il);
        explicit NullPtrEx(const char *t_argp);

        virtual ~NullPtrEx() = default;

    private:  /* Constructors (deleted) */
        NullPtrEx() = delete;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &t_os, const NullPtrEx &t_ex);

    public:  /* Methods */
        virtual void show() const override;
        virtual string name() const noexcept override;

    private:  /* Methods */
        string init_msg() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const NullPtrEx &t_ex)
    {
        return (t_os << static_cast<std::string>(t_ex));
    }
}

#endif // !NULL_PTR_EX_H
