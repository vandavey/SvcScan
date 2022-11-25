/*
*  runtime_ex.h
*  ------------
*  Header file for a runtime exception
*/
#pragma once

#ifndef RUNTIME_EX_H
#define RUNTIME_EX_H

#include <stdexcept>
#include "exception.h"

namespace scan
{
    /**
    * @brief  Runtime exception.
    */
    class RuntimeEx : public Exception
    {
    private:  /* Type Aliases */
        using base_t = Exception;

    private:  /* Constants */
        static constexpr cstr_t<16> NAME = { "scan::RuntimeEx" };  // Exception name

    public:  /* Fields */
        string caller;  // Method that threw exception

    public:  /* Constructors & Destructor */
        RuntimeEx() = delete;
        RuntimeEx(const RuntimeEx &t_ex);
        RuntimeEx(RuntimeEx &&) = delete;
        RuntimeEx(const string &t_caller, const string &t_msg);

        virtual ~RuntimeEx() = default;

    public:  /* Operators */
        RuntimeEx &operator=(const RuntimeEx &) = default;
        RuntimeEx &operator=(RuntimeEx &&) = default;

        operator std::string() const override;

        friend ostream &operator<<(ostream &t_os, const RuntimeEx &t_ex);

    public:  /* Methods */
        virtual void show() const;

        virtual string name() const noexcept;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const RuntimeEx &t_ex)
    {
        return t_os << static_cast<string>(t_ex);
    }
}

#endif // !RUNTIME_EX_H
