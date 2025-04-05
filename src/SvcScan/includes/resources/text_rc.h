/*
* @file
*     text_rc.h
* @brief
*     Header file for an embedded text file resource.
*/
#pragma once

#ifndef SCAN_TEXT_RC_H
#define SCAN_TEXT_RC_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <minwindef.h>
#include "../concepts/concepts.h"
#include "../errors/error_const_defs.h"
#include "../errors/logic_ex.h"
#include "../ranges/algo.h"
#include "../utils/aliases.h"
#include "../utils/const_defs.h"

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
        bool get_line(string& t_buffer, Unsigned auto t_ln_index) const;

        string& data() const;

    private:  /* Methods */
        static HMODULE get_module();

        void load_rc();
    };
}

/**
* @brief
*     Get a line from the underlying text data at the specified line
*     index. Returns true if the line data was successfully copied.
*/
inline bool scan::TextRc::get_line(string& t_buffer, Unsigned auto t_ln_index) const
{
    if (!m_loaded)
    {
        throw LogicEx{RC_NOT_LOADED_MSG, "TextRc::get_line"};
    }

    bool ln_found{false};
    size_t ln_index{static_cast<size_t>(t_ln_index)};

    if (ln_index < algo::count(*m_datap, LF))
    {
        const size_t beg_offset{algo::find_nth(*m_datap, LF, ln_index, true)};

        if (!algo::is_npos(beg_offset))
        {
            const size_t end_offset{algo::find_nth(*m_datap, LF, ln_index + 1_sz)};
            t_buffer = m_datap->substr(beg_offset, end_offset - beg_offset);

            ln_found = true;
        }
    }
    return ln_found;
}

#endif // !SCAN_TEXT_RC_H
