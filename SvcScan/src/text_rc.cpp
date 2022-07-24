/*
*  text_rc.h
*  ---------
*  Source file for an embedded text file resource
*/
#include <windows.h>
#include "includes/except/logic_ex.h"
#include "includes/except/runtime_ex.h"
#include "includes/resources/text_rc.h"

/**
* @brief  Initialize the object.
*/
scan::TextRc::TextRc()
{
    m_loaded = false;
    m_rc_symbol = NULL_SYMBOL;
}

/**
* @brief  Initialize the object.
*/
scan::TextRc::TextRc(TextRc &&t_trc) noexcept
{
    *this = std::forward<TextRc>(t_trc);
}

/**
* @brief  Initialize the object.
*/
scan::TextRc::TextRc(const symbol_t &t_symbol) : TextRc()
{
    *this = t_symbol;
}

/**
* @brief  Move assignment operator overload.
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
* @brief  Assignment operator overload.
*/
scan::TextRc &scan::TextRc::operator=(const symbol_t &t_symbol)
{
    m_rc_symbol = t_symbol;
    load_rc();

    return *this;
}

/**
* @brief  Get a line from the underlying text file data at the specified line index.
*/
bool scan::TextRc::get_line(string &t_line, const size_t &t_line_idx) const
{
    if (!m_loaded)
    {
        throw LogicEx{ "TextRc::get_line", "Resource must be loaded" };
    }

    bool line_found{ false };
    const size_t line_count{ Util::count(*m_datap, stdu::LF) };

    // Only extract substring when line index is valid
    if (t_line_idx < line_count)
    {
        const str_iterator beg = Util::find_nth(*m_datap,
                                                stdu::LF,
                                                t_line_idx,
                                                true);

        const str_iterator end{ Util::find_nth(*m_datap, stdu::LF, t_line_idx + 1) };

        // Error occurred while searching string data
        if (beg == m_datap->cend() || end == m_datap->cend())
        {
            throw RuntimeEx{ "TextRc::get_line", "Error occurred finding line" };
        }

        t_line = Util::substr(*m_datap, beg, end);
        line_found = true;
    }
    return line_found;
}

/**
* @brief  Get a constant reference to the underlying text file data.
*/
std::string &scan::TextRc::data() const
{
    return *m_datap;
}

/**
* @brief  Get a handle to the current module (application executable).
*/
HMODULE scan::TextRc::get_module()
{
    return GetModuleHandleA(nullptr);
}

/**
* @brief  Load the underlying embedded text file data from the application assembly.
*/
void scan::TextRc::load_rc()
{
    if (m_rc_symbol == NULL_SYMBOL)
    {
        throw LogicEx{ "TextRc::load_rc", "No resource symbol specified" };
    }

    if (!m_loaded)
    {
        const HMODULE module_handle{ get_module() };
        const char *symbolp{ MAKEINTRESOURCEA(m_rc_symbol) };

        // Locate resource info block
        HRSRC hrsrc_handle{ FindResourceA(module_handle, symbolp, &RC_TYPE[0]) };

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

        const size_t data_size{ SizeofResource(module_handle, hrsrc_handle) };
        const char *rcp{ reinterpret_cast<char *>(LockResource(hglobal_handle)) };

        // Resource is unavailable
        if (rcp == nullptr)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Requested resource unavailable" };
        }

        m_loaded = true;
        m_datap = std::make_unique<string>(std::string_view(rcp, data_size));
    }
}
