/*
*  argex.h
*  -------
*  Header file for invalid argument exceptions
*/
#pragma once

#ifndef ARG_EX_H
#define ARG_EX_H

#include <stdexcept>
#include <vector>
#include "../properties/autoprop.h"

namespace scan
{
    /// ***
    /// Standard argument exception
    /// ***
    class ArgEx : public std::invalid_argument
    {
    protected:  /* Types */
        using string   = std::string;
        using vector_s = std::vector<string>;

    private:  /* Types & Constants */
        using base = std::invalid_argument;

        static constexpr char NAME[] = "scan::ArgEx";

    public:  /* Fields */
        AutoProp<string> arg;  // Invalid argument
        AutoProp<string> msg;  // Error message

    public:  /* Constructors & Destructor */
        ArgEx(const ArgEx &) = default;
        ArgEx(const char *t_argp, const string &t_msg);
        ArgEx(const vector_s &t_vect, const string &t_msg);

        virtual ~ArgEx() = default;

    private:  /* Constructors (deleted) */
        ArgEx() = delete;

    public:  /* Operators */
        operator string() const;

        friend std::ostream &operator<<(std::ostream &t_os, const ArgEx &t_ex);

    public:  /* Methods */
        virtual void show() const;

        virtual string name() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const ArgEx &t_ex)
    {
        return (t_os << static_cast<std::string>(t_ex));
    }
}

#endif // !ARG_EX_H
