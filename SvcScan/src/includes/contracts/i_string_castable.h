/*
*  i_string_castable.h
*  -------------------
*  Header file for an interface that enforces string casting
*/
#pragma once

#ifndef I_STRING_CASTABLE_H
#define I_STRING_CASTABLE_H

#include <string>

namespace scan
{
    /**
    * @brief  Interface that enforces string casting mechanisms.
    */
    class IStringCastable
    {
    private:  /* Type Aliases */
        using string = std::string;

    public:  /* Constructors & Destructor */
        IStringCastable() = default;
        IStringCastable(const IStringCastable &) = default;

        virtual ~IStringCastable() = default;

    public:  /* Operators */
        virtual operator string() const = 0;
    };
}

#endif // !I_STRING_CASTABLE_H
