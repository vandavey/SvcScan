/*
*  textrc.h
*  --------
*  Header file for an embedded text file resource
*/
#pragma once

#ifndef TEXT_RC_H
#define TEXT_RC_H

#include <string>
#include <string_view>
#include "../resources.h"
#include "resource.h"

namespace scan
{
    /// ***
    /// Assembly embedded text file resource
    /// ***
    class TextRc final : public Resource
    {
    private: /* Types */
        using base = Resource;

        using string      = std::string;
        using string_view = std::string_view;

    private: /* Constants */
        static constexpr symbol RC_SYMBOL = CSV_DATA;  // Default symbol

        static constexpr char RC_TYPE[] = "TEXT";      // Resource type

    private: /* Fields */
        char *m_rc_ptr; // Resource pointer

    public: /* Constructors & Destructor */
        TextRc(const symbol &t_symbol = RC_SYMBOL);

        virtual ~TextRc() = default;

    private: /* Constructors (Deleted) */
        TextRc(const TextRc &) = delete;

    public: /* Methods */
        string data() const;

    private: /* Methods */
        void load() override;
    };
}

#endif // !TEXT_RC_H
