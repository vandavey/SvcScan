/*
*  resource.h
*  ----------
*  Header file for an embedded text assembly resource
*/
#pragma once

#ifndef RESOURCE_H
#define RESOURCE_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <string>
#include <string_view>
#include <windows.h>
#include "../resources.h"

namespace scan
{
    /// ***
    /// Embedded text assembly resource handler
    /// ***
    class Resource
    {
    private: /* Types */
        using string = std::string;

        using string_view = std::string_view;

    private: /* Constants */
        static constexpr char DEFAULT_TYPE[] = "TEXTFILE";  // Resource type

        static constexpr int DEFAULT_SYMBOL = CSV_DATA;     // Resource symbol

        static constexpr HMODULE MODULE{ nullptr };         // Module handle

    private: /* Fields */
        HRSRC m_rc_handle;     // Resource handle

        HGLOBAL m_mem_handle;  // Global memory handle

        size_t m_data_size;    // Resource size

        char *m_data_ptr;      // Pointer to the first 

    public: /* Constructors & Destructor */
        Resource();
        Resource(const int &t_symbol, const string &t_type);

        virtual ~Resource() = default;

    private: /* Constructors (Deleted) */
        Resource(const Resource &) = delete;

    public: /* Operators */
        operator string() const;

    public: /* Methods */
        string data() const;

    private: /* Methods */
        void load_resource(const int &t_symbol, const string &t_type);
    };
}

#endif // !RESOURCE_H
