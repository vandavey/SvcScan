/*
*  logicex.h
*  ---------
*  Header file for basic logic exception
*/
#pragma once

#ifndef LOGIC_EX_H
#define LOGIC_EX_h

#include <stdexcept>
#include <string>
#include "../properties/autoprop.h"

namespace scan
{
    /// ***
    /// Standard logic exception
    /// ***
    class LogicEx : public std::logic_error
    {
    private:  /* Types & Constants */
        using base   = std::logic_error;
        using string = std::string;

        static constexpr char NAME[] = "scan::LogicEx";

    public:  /* Fields */
        AutoProp<string> caller;  // Method that threw exception
        AutoProp<string> msg;     // Error message

    public:  /* Constructors & Destructor */
        LogicEx(const LogicEx &) = default;
        LogicEx(const string &t_caller, const string &t_msg);

        virtual ~LogicEx() = default;

    private:  /* Constructors (deleted) */
        LogicEx() = delete;

    public:  /* Operators */
        operator string() const;

        friend std::ostream &operator<<(std::ostream &t_os, const LogicEx &t_ex);

    public:  /* Methods */
        void show() const;

        string name() const noexcept;
    };

    /// ***
    /// Bitwise left shift operator overload
    /// ***
    inline std::ostream &operator<<(std::ostream &t_os, const LogicEx &t_ex)
    {
        return (t_os << static_cast<std::string>(t_ex));
    }
}

#endif // !LOGIC_EX_H

