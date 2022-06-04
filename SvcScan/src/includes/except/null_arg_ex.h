/*
*  null_arg_ex.h
*  -------------
*  Header file for a null argument exception
*/
#pragma once

#ifndef NULL_ARG_EX_H
#define NULL_ARG_EX_H

#include "arg_ex.h"

namespace scan
{
    /// ***
    /// Null argument exception
    /// ***
    class NullArgEx : public ArgEx
    {
    private:  /* Type Aliases */
        using base_t = ArgEx;

    private:  /* Constants */
        static constexpr char NAME[] = "scan::NullArgEx";

    public:  /* Constructors & Destructor */
        NullArgEx() = delete;
        NullArgEx(const NullArgEx &) = delete;
        NullArgEx(const char *t_argp);
        NullArgEx(const vector<string> &t_vect);

        virtual ~NullArgEx() = default;

    protected:  /* Constructors */
        NullArgEx(const vector<string> &t_vect, const string &t_msg);

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
