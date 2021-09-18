/*
*  resource.h
*  ----------
*  Header file for an assembly embedded resource
*/
#pragma once

#ifndef RESOURCE_H
#define RESOURCE_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace scan
{
    /// ***
    /// Abstract class for an assembly embedded resource
    /// ***
    class Resource
    {
    protected: /* Types */
        using symbol = int;

    protected: /* Fields */
        symbol m_rc_symbol;    // Resource symbol

        HRSRC m_rc_handle;     // Resource handle
        HGLOBAL m_mem_handle;  // Global memory handle

        size_t m_data_size;    // Resource size

    public: /* Destructor */
        virtual ~Resource() = default;

    protected: /* Constructors */
        explicit Resource(const symbol &t_symbol);

    private: /* Constructors (Deleted) */
        Resource() = delete;
        Resource(const Resource &) = delete;

    protected: /* Methods */
        static bool valid_symbol(const symbol &t_symbol);

        static HMODULE get_module();

        virtual void load() = 0;
    };
}

#endif // !RESOURCE_H
