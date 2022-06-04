/*
*  logic_ex.h
*  ----------
*  Header file for a basic logical exception
*/
#pragma once

#ifndef LOGIC_EX_H
#define LOGIC_EX_h

#include <iostream>
#include <stdexcept>
#include "exception.h"

namespace scan
{
    /// ***
    /// Standard logic exception
    /// ***
    class LogicEx : public Exception, public std::logic_error
    {
    private:  /* Type Aliases */
        using base_t = std::logic_error;

    private:  /* Constants */
        static constexpr char NAME[] = "scan::LogicEx";

    public:  /* Fields */
        string caller;  // Method that threw exception
        string msg;     // Error message

    public:  /* Constructors & Destructor */
        LogicEx() = delete;
        LogicEx(const LogicEx &) = delete;
        LogicEx(const string &t_caller, const string &t_msg);

        virtual ~LogicEx() = default;

    public:  /* Operators */
        operator string() const override;

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

