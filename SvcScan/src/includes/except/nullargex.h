/*
*  nullargex.h
*  -----------
*  Header file for null argument exceptions
*/
#pragma once

#ifndef NULLARG_H
#define NULLARG_H

#include <iostream>
#include "argex.h"

namespace Scan
{
    /// ***
    /// Null argument exception
    /// ***
    class NullArgEx : public ArgEx
    {
    private:  /* Types & Constants */
        typedef ArgEx base;
        typedef std::vector<std::string> vector_s;

        static constexpr char NAME[] = "SvcScan::Scan::NullArgEx";

    public:  /* Constructors & Destructor */
        NullArgEx(const NullArgEx &) = default;
        explicit NullArgEx(const char *sptr);
        explicit NullArgEx(const vector_s &vect);
        NullArgEx(const char *sptr, const bool &is_ptr);
        NullArgEx(const vector_s &vect, const bool &is_ptr);

        virtual ~NullArgEx() = default;

    private:  /* Constructors (deleted) */
        NullArgEx() = delete;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const NullArgEx &ex);

    public:  /* Methods */
        virtual void show() const;
        virtual const std::string name() const noexcept override;

    private:  /* Methods */
        const std::string init_msg(const vector_s &vect,
                                   const bool &is_ptr) const;
    };

    /// ***
    /// Bitwise left shift operator
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const NullArgEx &ex)
    {
        return (os << ex.str());
    }
}

#endif // !NULLARG_H
