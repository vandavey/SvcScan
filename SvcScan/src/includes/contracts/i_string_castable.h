/*
*  i_string_castable.h
*  -------------------
*  Header file for an interface that enforces string casting
*/
#pragma once

#ifndef I_STRING_CASTABLE_H
#define I_STRING_CASTABLE_H

#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Interface that enforces string casting mechanisms.
    */
    class IStringCastable
    {
    public:  /* Constructors & Destructor */
        IStringCastable() = default;
        IStringCastable(const IStringCastable &) = default;
        IStringCastable(IStringCastable &&) = default;

        virtual ~IStringCastable() = default;

    public:  /* Operators */
        IStringCastable &operator=(const IStringCastable &) = default;
        IStringCastable &operator=(IStringCastable &&) = default;

    public:  /* Operators */
        virtual operator string() const = 0;
    };
}

#endif // !I_STRING_CASTABLE_H
