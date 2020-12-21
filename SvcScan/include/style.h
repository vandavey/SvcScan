#pragma once

#ifndef STYLE_H
#define STYLE_H

#include <string>
#include "autoprop.h"

#if defined(_WIN32) && !defined(WIN_OS)
#  define WIN_OS
#elif !defined(NIX_OS)
#  define NIX_OS
#endif// _WIN32 && !WIN_OS

namespace Scan
{
    /// ***
    /// Ansi escape sequence stylist
    /// ***
    class Style
    {
    private: /* Constants & Types */
        typedef unsigned long ulong;

        // Ansi escape sequences
        static constexpr wchar_t CYAN[] = L"\033[38;2;0;255;255m";
        static constexpr wchar_t RED[] = L"\033[38;2;246;0;0m";
        static constexpr wchar_t RESET[] = L"\033[0m";
        static constexpr wchar_t YELLOW[] = L"\033[38;2;0;255;255m";

    public: /* Properties */
        static AutoProp<bool> vt_mode; // VT processing

    private: /* Constructors & Destructor */
        Style() = delete;
        Style(const Style &style) = delete;
        virtual ~Style() = default;

    public: /* Methods */
        static void error(const std::string &msg);
        static void error(const std::string &msg, const std::string &arg);
        static void print(const std::string &msg);
        static void warning(const std::string &msg);

        static const int enable_vtmode();

    #ifdef WIN_OS
        static const std::wstring utf16(const std::string &data);
    #endif// !WIN_OS

    private: /* Methods */
        template<class T>
        static const std::string fmt(const std::string &msg, const T &arg);
    };
}

#endif// !STYLE_H
