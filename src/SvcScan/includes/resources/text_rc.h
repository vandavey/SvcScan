/*
* @file
*     text_rc.h
* @brief
*     Header file for an embedded text file resource.
*/
#pragma once

#ifndef SCAN_TEXT_RC_H
#define SCAN_TEXT_RC_H

#include <minwindef.h>
#include "../utils/aliases.h"

namespace scan
{
    /**
    * @brief
    *     Assembly embedded text file resource.
    */
    class TextRc final
    {
    private:  /* Constants */
        static constexpr int INVALID_SYMBOL = 0;       // Invalid resource symbol

        static constexpr c_string_t RC_TYPE = "TEXT";  // Resource type

    private:  /* Fields */
        bool m_loaded;               // Resource loaded
        int m_symbol;                // Resource symbol

        unique_ptr<string> m_datap;  // Text data smart pointer

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr TextRc() noexcept
        {
            m_loaded = false;
            m_symbol = INVALID_SYMBOL;
        }

        TextRc(const TextRc&) = delete;
        TextRc(TextRc&&) = default;
        TextRc(int t_symbol);

        virtual ~TextRc() = default;

    public:  /* Operators */
        TextRc& operator=(const TextRc&) = delete;
        TextRc& operator=(TextRc&&) = default;

    public:  /* Methods */
        bool get_line(string& t_ln_buffer, size_t t_ln_index) const;

        string& data() const;

    private:  /* Methods */
        static HMODULE get_module();

        void load_rc();
    };
}

#endif // !SCAN_TEXT_RC_H
