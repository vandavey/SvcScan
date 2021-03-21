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

namespace Scan
{
    class ArgEx : public std::invalid_argument
    {
    protected:  /* Types */
        using string = std::string;
        using vector_s = std::vector<string>;

    private:  /* Types & Constants */
        using base = std::invalid_argument;
        static constexpr char NAME[] = "SvcScan::Scan::ArgEx";

    public:  /* Fields */
        AutoProp<string> arg;  // Invalid argument
        AutoProp<string> msg;  // Error message

    public:  /* Constructors & Destructor */
        ArgEx(const ArgEx &) = default;
        ArgEx(const char *argp, const string &msg);
        ArgEx(const vector_s &vect, const string &msg);

        virtual ~ArgEx() = default;

    private:  /* Constructors (deleted) */
        ArgEx() = delete;

    public:  /* Operators */
        operator const std::string() const;
        operator std::string();

        friend std::ostream &operator<<(std::ostream &os, const ArgEx &ex);

    public:  /* Methods */
        virtual void show() const;

        virtual const string name() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const ArgEx &ex)
    {
        return (os << static_cast<std::string>(ex));
    }
}

#endif // !ARG_EX_H
