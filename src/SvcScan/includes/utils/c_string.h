/*
* @file
*     c_string.cpp
* @brief
*     Source file for for constant evaluated integer to string conversions.
*/
#pragma once

#ifndef SCAN_C_STRING_H
#define SCAN_C_STRING_H

#include "../concepts/concepts.h"
#include "expr.h"

namespace scan
{
    /**
    * @brief
    *     Calculate the buffer size needed to store an integral value as a C-string.
    */
    template<size_t N>
    static consteval size_t buffer_length() noexcept
    {
        size_t length{ 1U };

        if constexpr (N > 0)
        {
            for (size_t i{ N }; i != 0; i /= 10U)
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
    class CString
    {
    private:  /* Constants */
        static constexpr size_t LENGTH = buffer_length<N>();  // C-string buffer length

        static constexpr cstr_t NUMERIC_CHARS = "0123456789"; // Numeric characters

    private:  /* Fields */
        char m_buffer[LENGTH];  // C-string buffer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr CString() noexcept : m_buffer{ CHAR_NULL }
        {
        #pragma warning(push)
        #pragma warning(disable: 6385) // Avoid buffer overruns
            Pointer auto ptr{ &m_buffer[0] + sizeof(m_buffer) };
            *--ptr = CHAR_NULL;

            if constexpr (N > 0)
            {
                for (size_t i{ N }; i != 0; i /= 10U)
                {
                    *--ptr = NUMERIC_CHARS[i % 10U];
                }
            }
            else if constexpr (N == 0)
            {
                m_buffer[0] = '0';
            }
        #pragma warning(pop)
        }

        constexpr CString(const CString &) = default;
        constexpr CString(CString &&) = default;

        constexpr virtual ~CString() = default;

    public:  /* Operators */
        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator const char *() const noexcept
        {
            return &m_buffer[0];
        }

    public:  /* Methods */
        /**
        * @brief
        *     Get a constant pointer to the first element of the underlying C-string.
        */
        constexpr const char *data() const noexcept
        {
            return &m_buffer[0];
        }
    };
}

#endif // !SCAN_C_STRING_H
