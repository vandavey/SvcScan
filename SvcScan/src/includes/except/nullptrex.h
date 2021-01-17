/*
*  nullptrex.h
*  -----------
*  Header file for nullptr argument exceptions
*/
#pragma once

#ifndef NULLPTREX_H
#define NULLPTREX_H

#include "nullargex.h"

namespace Scan
{
    /// ***
    /// Null pointer argument exception
    /// ***
    class NullPtrEx : public NullArgEx
    {
    private:  /* Types & Constants */
        typedef NullArgEx base;
        static constexpr char NAME[] = "SvcScan::Scan::NullPtrEx";

    public:  /* Constructors & Destructor */
        NullPtrEx(const NullPtrEx &) = default;
        explicit NullPtrEx(const char *argp);
        explicit NullPtrEx(const vector_s &vect);

        virtual ~NullPtrEx() = default;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const NullArgEx &ex);

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

#endif // !NULLPTREX_H
