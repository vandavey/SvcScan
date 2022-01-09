/*
*  nullargex.h
*  -----------
*  Header file for null argument exceptions
*/
#pragma once

#ifndef NULL_ARG_EX_H
#define NULL_ARG_EX_H

#include "argex.h"

namespace scan
{
    /// ***
    /// Null argument exception
    /// ***
    class NullArgEx : public ArgEx
    {
    private:  /* Types & Constants */
        using base = ArgEx;

        static constexpr char NAME[] = "scan::NullArgEx";

    public:  /* Constructors & Destructor */
        NullArgEx(const char *t_argp);
        NullArgEx(const vector_s &t_vect);

        virtual ~NullArgEx() = default;

    protected:  /* Constructors */
        NullArgEx(const vector_s &t_vect, const string &t_msg);

    private:  /* Constructors (deleted) */
        NullArgEx() = delete;
        NullArgEx(const NullArgEx &) = default;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &t_os, const NullArgEx &t_ex);

    public:  /* Methods */
        virtual void show() const override;
        virtual string name() const noexcept override;

    private:  /* Methods */
        string init_msg() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const NullArgEx &t_ex)
    {
        return (t_os << static_cast<std::string>(t_ex));
    }
}

#endif // !NULL_ARG_EX_H
