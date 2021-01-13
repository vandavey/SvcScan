/*
*  argex.h
*  -------
*  Header file for invalid argument exceptions
*/
#pragma once

#ifndef ARGEX_H
#define ARGEX_H

#include <stdexcept>
#include <string>
#include <vector>
#include "../properties/autoprop.h"

namespace Scan
{
    class ArgEx : std::invalid_argument
    {
    private:  /* Types */
        typedef std::invalid_argument base;
        typedef std::vector<std::string> vector_s;

    public:  /* Constants */
        static constexpr char NAME[] = "SvcScan::Scan::ArgEx";  // Exception

    public:  /* Fields */
        AutoProp<std::string> arg;  // Invalid argument
        AutoProp<std::string> msg;  // Error message

    public:  /* Constructors & Destructor */
        ArgEx(const ArgEx &ex);
        ArgEx(const std::string &arg, const std::string &msg);
        virtual ~ArgEx() = default;

    protected:  /* Constructors */
        explicit ArgEx(const vector_s &vect, const std::string &msg);

    private:  /* Constructors (deleted) */
        ArgEx() = delete;

    public:  /* Operators */
        friend std::ostream &operator<<(std::ostream &os, const ArgEx &ex);

    public:  /* Methods */
        void show() const;
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
