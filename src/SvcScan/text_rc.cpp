/*
* @file
*     text_rc.cpp
* @brief
*     Source file for an embedded text file resource.
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <memory>
#include <string>
#include <utility>
#include <windows.h>
#include <libloaderapi.h>
#include <winbase.h>
#include <winuser.h>
#include "includes/errors/error_const_defs.h"
#include "includes/errors/logic_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/ranges/algo.h"
#include "includes/resources/text_rc.h"
#include "includes/utils/const_defs.h"
#include "includes/utils/literals.h"

/**
* @brief
*     Initialize the object.
*/
scan::TextRc::TextRc() noexcept
{
    m_loaded = false;
    m_rc_symbol = INVALID_SYMBOL;
}

/**
* @brief
*     Initialize the object.
*/
scan::TextRc::TextRc(TextRc&& t_trc) noexcept
{
    *this = std::move(t_trc);
}

/**
* @brief
*     Initialize the object.
*/
scan::TextRc::TextRc(symbol_t t_symbol) : TextRc{}
{
    *this = t_symbol;
}

/**
* @brief
*     Move assignment operator overload.
*/
scan::TextRc& scan::TextRc::operator=(TextRc&& t_trc) noexcept
{
    if (this != &t_trc)
    {
        m_datap = std::move(t_trc.m_datap);
        m_loaded = t_trc.m_loaded;
        m_rc_symbol = t_trc.m_rc_symbol;
    }
    return *this;
}

/**
* @brief
*     Assignment operator overload.
*/
scan::TextRc& scan::TextRc::operator=(symbol_t t_symbol)
{
    m_rc_symbol = t_symbol;
    load_rc();

    return *this;
}

/**
* @brief
*     Get a line from the underlying text data at the specified line
*     index. Returns true if the line data was successfully copied.
*/
bool scan::TextRc::get_line(string& t_ln_buffer, size_t t_ln_index) const
{
    if (!m_loaded)
    {
        throw LogicEx{RC_NOT_LOADED_MSG, "TextRc::get_line"};
    }
    bool ln_found{false};

    if (t_ln_index < algo::count(*m_datap, LF))
    {
        const size_t beg_offset{algo::find_nth(*m_datap, LF, t_ln_index, true)};

        if (!algo::is_npos(beg_offset))
        {
            const size_t end_offset{algo::find_nth(*m_datap, LF, t_ln_index + 1_sz)};
            t_ln_buffer = m_datap->substr(beg_offset, end_offset - beg_offset);

            ln_found = true;
        }
    }
    return ln_found;
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
    if (m_rc_symbol == INVALID_SYMBOL)
    {
        throw LogicEx{INVALID_RC_SYMBOL_MSG, "TextRc::load_rc"};
    }

    if (!m_loaded)
    {
        const HMODULE module_handle{get_module()};
        const char* symbol_ptr{MAKEINTRESOURCEA(m_rc_symbol)};

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
