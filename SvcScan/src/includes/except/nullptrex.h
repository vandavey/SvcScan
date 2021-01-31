/*
*  nullptrex.h
*  -----------
*  Header file for nullptr argument exceptions
*/
#pragma once

#ifndef NULL_PTR_EX_H
#define NULL_PTR_EX_H

#include "nullargex.h"

namespace Scan
{
    /// ***
    /// Null pointer argument exception
    /// ***
    class NullPtrEx : public NullArgEx
    {
    private:  /* Types & Constants */
        using base = NullArgEx;
        static constexpr char NAME[] = "SvcScan::Scan::NullPtrEx";

    public:  /* Constructors & Destructor */
        NullPtrEx(const NullPtrEx &) = default;
        explicit NullPtrEx(const char *argp);
        explicit NullPtrEx(const vector_s &vect);

        virtual ~NullPtrEx() = default;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const NullPtrEx &ex);

    public:  /* Methods */
        virtual void show() const override;
        virtual const std::string name() const noexcept override;

    private:  /* Methods */
        const std::string init_msg() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const NullPtrEx &ex)
    {
        return (os << ex.str());
    }
}

#endif // !NULL_PTR_EX_H
