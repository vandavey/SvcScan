/*
* @file
*     i_string_castable.h
* @brief
*     Header file for an interface that enforces string casting.
*/
#pragma once

#ifndef SCAN_I_STRING_CASTABLE_H
#define SCAN_I_STRING_CASTABLE_H

#include "../utils/alias.h"

namespace scan
{
    /**
    * @brief
    *     Interface that enforces string casting mechanisms.
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

#endif // !SCAN_I_STRING_CASTABLE_H
