/*
* @file
*     c_string.h
* @brief
*     Header file for constant evaluated integer to string conversions.
*/
#pragma once

#ifndef SCAN_C_STRING_H
#define SCAN_C_STRING_H

#include "../concepts/concepts.h"
#include "aliases.h"
#include "const_defs.h"
#include "literals.h"

namespace scan
{
    /**
    * @brief
    *     Calculate the buffer size needed to store an integral value as a C-string.
    */
    template<size_t N>
    consteval size_t buffer_length() noexcept
    {
        size_t length{1_st};

        if constexpr (N > 0)
        {
            for (size_t i{N}; i != 0; i /= 10_st)
            {
                length++;
            }
        }
        return length;
    }

    /**
    * @brief
    *     C-string wrapper for constant evaluated integer to string conversions.
    */
    template<size_t N>
    class CString final
    {
    public:  /* Constants */
        static constexpr size_t LEN = buffer_length<N>();  // Array buffer length

    private:  /* Constants */
        static constexpr c_string_t NUM_CHARS = "0123456789";  // Numeric characters

    private:  /* Fields */
        char m_buffer[LEN];  // C-string (array) buffer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr CString() noexcept : m_buffer{CHAR_NULL}
        {
            Pointer auto ptr{&m_buffer[0] + sizeof m_buffer};
            *--ptr = CHAR_NULL;

            if constexpr (N > 0)
            {
                for (size_t i{N}; i != 0; i /= 10_st)
                {
                    *--ptr = NUM_CHARS[i % 10_st];
                }
            }
            else if constexpr (N == 0)
            {
                m_buffer[0] = NUM_CHARS[N];
            }
        }

        constexpr CString(const CString&) = default;
        constexpr CString(CString&&) = default;

        virtual constexpr ~CString() = default;

    public:  /* Operators */
        constexpr CString& operator=(const CString&) = default;
        constexpr CString& operator=(CString&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const noexcept
        {
            return static_cast<string>(m_buffer);
        }

    public:  /* Methods */
        /**
        * @brief
        *     Get a constant pointer to the first character of the underlying array.
        */
        constexpr const char* data() const noexcept
        {
            return &m_buffer[0];
        }
    };
}

#endif // !SCAN_C_STRING_H
