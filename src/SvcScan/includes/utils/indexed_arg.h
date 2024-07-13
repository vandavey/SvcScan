/*
* @file
*     indexed_arg.h
* @brief
*     Header file for an indexed command-line argument.
*/
#pragma once

#ifndef SCAN_INDEXED_ARG_H
#define SCAN_INDEXED_ARG_H

#include "aliases.h"
#include "literals.h"

namespace scan
{
    /**
    * @brief
    *     Generic indexed command-line argument.
    */
    class IndexedArg
    {
    public:  /* Fields */
        size_t index;  // Argument index
        string value;  // Argument value

    public:  /* Constructors & Destructor */
        constexpr IndexedArg() noexcept : index{ 0_st }
        {
        }

        constexpr IndexedArg(const IndexedArg &) = default;
        constexpr IndexedArg(IndexedArg &&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr IndexedArg(const size_t &t_idx, const string &t_value) noexcept
        {
            index = t_idx;
            value = t_value;
        }

        virtual constexpr ~IndexedArg() = default;

    public:  /* Operators */
        constexpr IndexedArg &operator=(const IndexedArg &) = default;
        constexpr IndexedArg &operator=(IndexedArg &&) = default;
    };
}

#endif // !SCAN_INDEXED_ARG_H
