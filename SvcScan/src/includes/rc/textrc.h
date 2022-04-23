/*
*  textrc.h
*  --------
*  Header file for an embedded text file resource
*/
#pragma once

#ifndef TEXT_RC_H
#define TEXT_RC_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "../rc/resource.h"

namespace scan
{
    /// ***
    /// Assembly embedded text file resource
    /// ***
    class TextRc final
    {
    private:  /* Type Aliases */
        using symbol = int;

        using string      = std::string;
        using string_view = std::string_view;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Constants */
        static constexpr char RC_TYPE[] = "TEXT";  // Resource type

    private:  /* Fields */
        bool m_loaded;         // Resource loaded

        symbol m_rc_symbol;    // Resource symbol
        char *m_rc_ptr;        // Resource pointer

        HRSRC m_rc_handle;     // Resource handle
        HGLOBAL m_mem_handle;  // Global memory handle

        size_t m_data_size;    // Resource size

    public:  /* Constructors & Destructor */
        TextRc();
        explicit TextRc(const symbol &t_symbol);

        virtual ~TextRc() = default;

    private:  /* Constructors (Deleted) */
        TextRc(const TextRc &) = delete;

    public:  /* Operators */
        TextRc &operator=(const symbol &t_symbol);

    public:  /* Methods */
        bool get_line(string &t_line, const size_t &t_line_idx) const;

        string data() const;

    private:  /* Methods */
        static HMODULE get_module();

        void load_rc();
    };
}

#endif // !TEXT_RC_H
