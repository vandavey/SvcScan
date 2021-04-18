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

        static constexpr char NAME[] = "SvcScan::scan::NullPtrEx";

    public:  /* Constructors & Destructor */
        NullPtrEx(const NullPtrEx &) = default;
        explicit NullPtrEx(const char *t_argp);
        explicit NullPtrEx(const vector_s &t_vect);

        virtual ~NullPtrEx() = default;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &t_os,
                                        const NullPtrEx &t_ex);

    public:  /* Methods */
        virtual void show() const override;
        virtual const std::string name() const noexcept override;

    private:  /* Methods */
        const std::string init_msg() const noexcept;
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
