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
    private:  /* Types */
        typedef ArgEx base;
        typedef std::vector<std::string> vector_s;

    public:  /* Constants */
        static constexpr char NAME[] = "SvcScan::Scan::NullArgEx";  // Exception

    private:  /* Fields */
        bool m_isptr;

    public:  /* Constructors & Destructor */
        NullArgEx(const NullArgEx &ex);
        explicit NullArgEx(const vector_s &vect);
        NullArgEx(const vector_s  &vect, const bool &is_ptr);

        virtual ~NullArgEx() = default;

    private:  /* Constructors (deleted) */
        NullArgEx() = delete;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const NullArgEx &ex);

    public:  /* Methods */
        void show() const;
        const std::string str() const override;
        std::string str() override;

    private:  /* Methods */
        virtual const std::string get_msg(const vector_s &vect,
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
