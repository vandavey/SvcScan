/*
* @file
*     text_rc.cpp
* @brief
*     Source file for an embedded text file resource.
*/
#include <memory>
#include <string>
#include <windows.h>
#include <libloaderapi.h>
#include <winbase.h>
#include <winuser.h>
#include "includes/errors/runtime_ex.h"
#include "includes/resources/text_rc.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Initialize the object.
*/
scan::TextRc::TextRc(int t_symbol) : TextRc{}
{
    m_symbol = t_symbol;
    load_rc();
}

/**
* @brief
*     Get a constant reference to the underlying text file data.
*/
std::string& scan::TextRc::data() const
{
    return *m_datap;
}

/**
* @brief
*     Get a handle to the current module (application executable).
*/
HMODULE scan::TextRc::get_module()
{
    return GetModuleHandleA(nullptr);
}

/**
* @brief
*     Load the underlying embedded text file data from the application assembly.
*/
void scan::TextRc::load_rc()
{
    if (m_symbol == INVALID_SYMBOL)
    {
        throw LogicEx{INVALID_RC_SYMBOL_MSG, "TextRc::load_rc"};
    }

    if (!m_loaded)
    {
        const HMODULE module_handle{get_module()};
        const char* symbol_ptr{MAKEINTRESOURCEA(m_symbol)};

        // Locate resource info block
        HRSRC hrsrc_handle{FindResourceA(module_handle, symbol_ptr, RC_TYPE)};

        if (hrsrc_handle == nullptr)
        {
            throw RuntimeEx{RC_LOAD_FAILED_MSG, "TextRc::load_rc"};
        }

        // Acquire resource handle
        HGLOBAL hglobal_handle{LoadResource(module_handle, hrsrc_handle)};

        if (hglobal_handle == nullptr)
        {
            throw RuntimeEx{RC_LOAD_FAILED_MSG, "TextRc::load_rc"};
        }
        const ulong_t rc_size{SizeofResource(module_handle, hrsrc_handle)};

        // Failed to get resource size
        if (rc_size == 0UL)
        {
            throw RuntimeEx{RC_LOAD_FAILED_MSG, "TextRc::load_rc"};
        }
        const char* rc_ptr{static_cast<char*>(LockResource(hglobal_handle))};

        // Resource is unavailable
        if (rc_ptr == nullptr)
        {
            throw RuntimeEx{RC_LOAD_FAILED_MSG, "TextRc::load_rc"};
        }

        m_loaded = true;
        m_datap = std::make_unique<string>(rc_ptr, rc_size);
    }
}
