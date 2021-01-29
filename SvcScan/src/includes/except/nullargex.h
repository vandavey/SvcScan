/*
*  nullargex.h
*  -----------
*  Header file for null argument exceptions
*/
#pragma once

#ifndef NULLARG_H
#define NULLARG_H

#include "argex.h"

namespace Scan
{
    /// ***
    /// Null argument exception
    /// ***
    class NullArgEx : public ArgEx
    {
    private:  /* Types & Constants */
        using base = ArgEx;
        static constexpr char NAME[] = "SvcScan::Scan::NullArgEx";

    public:  /* Constructors & Destructor */
        NullArgEx(const NullArgEx &) = default;
        explicit NullArgEx(const char *arg_ptr);
        explicit NullArgEx(const vector_s &vect);

        virtual ~NullArgEx() = default;

    protected:  /* Constructors */
        NullArgEx(const vector_s &vect, const std::string &msg);

    private:  /* Constructors (deleted) */
        NullArgEx() = delete;

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
    inline std::ostream &operator<<(std::ostream &os, const NullArgEx &ex)
    {
        return (os << ex.str());
    }
}

#endif // !NULLARG_H
