/*
*  text_rc.h
*  ---------
*  Header file for an embedded text file resource
*/
#pragma once

#ifndef TEXT_RC_H
#define TEXT_RC_H

#include "../io/std_util.h"

namespace scan
{
    /**
    * @brief  Assembly embedded text file resource.
    */
    class TextRc final
    {
    private:  /* Type Aliases */
        using this_t = TextRc;

        using symbol_t = int;

        using stdu         = StdUtil;
        using str_iterator = std::string::const_iterator;
        using string       = std::string;

        template<class T>
        using unique_ptr = std::unique_ptr<T>;

    private:  /* Constants */
        static constexpr symbol_t NULL_SYMBOL{ 0 };  // Null resource symbol

        static constexpr char RC_TYPE[] = "TEXT";    // Resource type

    private:  /* Fields */
        bool m_loaded;               // Resource loaded
        symbol_t m_rc_symbol;        // Resource symbol

        unique_ptr<string> m_datap;  // Text data smart pointer

    public:  /* Constructors & Destructor */
        TextRc();
        TextRc(const TextRc &) = default;
        TextRc(TextRc &&t_trc) noexcept;
        TextRc(const symbol_t &t_symbol);

        virtual ~TextRc() = default;

    public:  /* Operators */
        TextRc &operator=(const TextRc &) = default;
        TextRc &operator=(TextRc &&t_trc) noexcept;
        TextRc &operator=(const symbol_t &t_symbol);

    public:  /* Methods */
        bool get_line(string &t_line, const size_t &t_line_idx) const;

        string &data() const;

    private:  /* Methods */
        static HMODULE get_module();

        void load_rc();
    };
}

#endif // !TEXT_RC_H
