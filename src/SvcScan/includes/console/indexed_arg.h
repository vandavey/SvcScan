/*
* @file
*     indexed_arg.h
* @brief
*     Header file for an indexed command-line argument.
*/
#pragma once

#ifndef SCAN_INDEXED_ARG_H
#define SCAN_INDEXED_ARG_H

#include "../utils/aliases.h"
#include "../utils/literals.h"

namespace scan
{
    /**
    * @brief
    *     Indexed command-line argument.
    */
    class IndexedArg
    {
    public:  /* Fields */
        size_t index;  // Argument index
        string value;  // Argument value

    public:  /* Constructors & Destructor */
        constexpr IndexedArg() noexcept : index{0_sz}
        {
        }

        constexpr IndexedArg(const IndexedArg&) = default;
        constexpr IndexedArg(IndexedArg&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr IndexedArg(size_t t_index, const string& t_value) noexcept
        {
            index = t_index;
            value = t_value;
        }

        virtual constexpr ~IndexedArg() = default;

    public:  /* Operators */
        constexpr IndexedArg& operator=(const IndexedArg&) = default;
        constexpr IndexedArg& operator=(IndexedArg&&) = default;
    };
}

#endif // !SCAN_INDEXED_ARG_H
