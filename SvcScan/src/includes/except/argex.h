/*
*  argex.h
*  -------
*  Header file for invalid argument exceptions
*/
#pragma once

#ifndef ARGEX_H
#define ARGEX_H

#include <stdexcept>
#include <vector>
#include "../properties/autoprop.h"

namespace Scan
{
    class ArgEx : public std::invalid_argument
    {
    protected:  /* Types */
        typedef std::vector<std::string> vector_s;

    private:  /* Types & Constants */
        typedef std::invalid_argument base;
        static constexpr char NAME[] = "SvcScan::Scan::ArgEx";

    public:  /* Fields */
        AutoProp<std::string> arg;  // Invalid argument
        AutoProp<std::string> msg;  // Error message

    public:  /* Constructors & Destructor */
        ArgEx(const ArgEx &) = default;
        ArgEx(const char *argp, const std::string &msg);
        ArgEx(const vector_s &vect, const std::string &msg);

        virtual ~ArgEx() = default;

    private:  /* Constructors (deleted) */
        ArgEx() = delete;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const ArgEx &ex);

    public:  /* Methods */
        virtual void show() const;

        virtual const std::string name() const noexcept;
        virtual const std::string str() const;
        virtual std::string str();
    };

    /// ***
    /// Bitwise left shift operator
    /// ***
    inline std::ostream &operator<<(std::ostream &os, const ArgEx &ex)
    {
        return (os << ex.str());
    }
}

#endif // !ARGEX_H
