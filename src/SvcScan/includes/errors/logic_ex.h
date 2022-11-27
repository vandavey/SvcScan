/*
*  logic_ex.h
*  ----------
*  Header file for a logic exception
*/
#pragma once

#ifndef LOGIC_EX_H
#define LOGIC_EX_h

#include "exception.h"

namespace scan
{
    /**
    * @brief  Logic exception.
    */
    class LogicEx : public Exception
    {
    private:  /* Type Aliases */
        using base_t = Exception;

    private:  /* Constants */
        static constexpr cstr_t<14> NAME = { "scan::LogicEx" };  // Exception name

    public:  /* Fields */
        string caller;  // Method that threw exception

    public:  /* Constructors & Destructor */
        LogicEx() = delete;
        LogicEx(const LogicEx &t_ex);
        LogicEx(LogicEx &&) = delete;
        LogicEx(const string &t_caller, const string &t_msg);

        virtual ~LogicEx() = default;

    public:  /* Operators */
        LogicEx &operator=(const LogicEx &) = default;
        LogicEx &operator=(LogicEx &&) = default;

        operator std::string() const override;

        friend ostream &operator<<(ostream &t_os, const LogicEx &t_ex);

    public:  /* Methods */
        void show() const;

        string name() const noexcept;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const LogicEx &t_ex)
    {
        return t_os << static_cast<string>(t_ex);
    }
}

#endif // !LOGIC_EX_H

