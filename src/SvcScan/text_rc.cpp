/*
* @file
*     text_rc.cpp
* @brief
*     Source file for an embedded text file resource.
*/
#include <memory>
#include <string>
#include <type_traits>
#include <windows.h>
#include <libloaderapi.h>
#include <winbase.h>
#include <winuser.h>
#include "includes/concepts/concepts.h"
#include "includes/errors/logic_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/resources/text_rc.h"
#include "includes/utils/algorithm.h"
#include "includes/utils/const_defs.h"

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
scan::TextRc::TextRc(TextRc &&t_trc) noexcept
{
    *this = std::move(t_trc);
}

/**
* @brief
*     Initialize the object.
*/
scan::TextRc::TextRc(const symbol_t &t_symbol) : TextRc()
{
    *this = t_symbol;
}

/**
* @brief
*     Move assignment operator overload.
*/
scan::TextRc &scan::TextRc::operator=(TextRc &&t_trc) noexcept
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
scan::TextRc &scan::TextRc::operator=(const symbol_t &t_symbol)
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
bool scan::TextRc::get_line(string &t_ln_buffer, const size_t &t_ln_idx) const
{
    if (!m_loaded)
    {
        throw LogicEx{ "TextRc::get_line", "Resource must be loaded" };
    }
    bool ln_found{ false };

    if (t_ln_idx < algo::count(*m_datap, LF))
    {
        const size_t beg_offset{ algo::find_nth(*m_datap, LF, t_ln_idx, true) };
        const size_t end_offset{ algo::find_nth(*m_datap, LF, t_ln_idx + 1) };

        if (beg_offset == string::npos)
        {
            throw RuntimeEx{ "TextRc::get_line", "Error occurred finding line" };
        }

        ln_found = true;
        t_ln_buffer = m_datap->substr(beg_offset, end_offset - beg_offset);
    }
    return ln_found;
}

/**
* @brief
*     Get a constant reference to the underlying text file data.
*/
std::string &scan::TextRc::data() const
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
        throw LogicEx{ "TextRc::load_rc", "No resource symbol specified" };
    }

    if (!m_loaded)
    {
        const HMODULE module_handle{ get_module() };
        const char *symbolp{ MAKEINTRESOURCEA(m_rc_symbol) };

        // Locate resource info block
        HRSRC hrsrc_handle{ FindResourceA(module_handle, symbolp, RC_TYPE) };

        if (hrsrc_handle == nullptr)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Failed to find resource" };
        }

        // Acquire resource handle
        HGLOBAL hglobal_handle{ LoadResource(module_handle, hrsrc_handle) };

        if (hglobal_handle == nullptr)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Failed to get resource handle" };
        }
        const ulong_t data_size{ SizeofResource(module_handle, hrsrc_handle) };

        // Failed to get resource size
        if (data_size == 0UL)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Failed to get resource size" };
        }
        const char *rcp{ static_cast<char *>(LockResource(hglobal_handle)) };

        // Resource is unavailable
        if (rcp == nullptr)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Requested resource unavailable" };
        }

        m_loaded = true;
        m_datap = std::make_unique<string>(rcp, data_size);
    }
}
