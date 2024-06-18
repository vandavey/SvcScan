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
#include "../io/std_util.h"
#include "../utils/alias.h"

namespace scan
{
    /**
    * @brief
    *     Assembly embedded text file resource.
    */
    class TextRc final
    {
    private:  /* Type Aliases */
        using symbol_t = int;

        using stdu = StdUtil;

    private:  /* Constants */
        static constexpr symbol_t INVALID_SYMBOL = 0;  // Invalid resource symbol

        static constexpr cstr_t RC_TYPE = "TEXT";      // Resource type

    private:  /* Fields */
        bool m_loaded;               // Resource loaded
        symbol_t m_rc_symbol;        // Resource symbol

        unique_ptr<string> m_datap;  // Text data smart pointer

    public:  /* Constructors & Destructor */
        TextRc() noexcept;
        TextRc(const TextRc &) = default;
        TextRc(TextRc &&t_trc) noexcept;
        TextRc(const symbol_t &t_symbol);

        virtual ~TextRc() = default;

    public:  /* Operators */
        TextRc &operator=(const TextRc &) = default;
        TextRc &operator=(TextRc &&t_trc) noexcept;
        TextRc &operator=(const symbol_t &t_symbol);

    public:  /* Methods */
        bool get_line(string &t_ln_buffer, const size_t &t_ln_idx) const;

        string &data() const;

    private:  /* Methods */
        static HMODULE get_module();

        void load_rc();
    };
}

#endif // !SCAN_TEXT_RC_H
