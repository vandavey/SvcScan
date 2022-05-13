/*
*  text_rc.h
*  ---------
*  Source file for an embedded text file resource
*/
#include <windows.h>
#include "includes/except/logic_ex.h"
#include "includes/except/runtime_ex.h"
#include "includes/resources/text_rc.h"

/// ***
/// Initialize the object
/// ***
scan::TextRc::TextRc()
{
    m_loaded = false;
    m_rc_symbol = NULL;
}

/// ***
/// Initialize the object
/// ***
scan::TextRc::TextRc(TextRc &&t_trc) noexcept
{
    operator=(std::forward<TextRc>(t_trc));
}

/// ***
/// Initialize the object
/// ***
scan::TextRc::TextRc(const symbol_t &t_symbol) : TextRc()
{
    m_rc_symbol = t_symbol;
    load_rc();
}

/// ***
/// Assignment operator overload
/// ***
scan::TextRc &scan::TextRc::operator=(TextRc &&t_trc) noexcept
{
    m_datap = std::move(t_trc.m_datap);
    m_loaded = t_trc.m_loaded;
    m_rc_symbol = t_trc.m_rc_symbol;

    return *this;
}

/// ***
/// Retrieve the specified line from the embedded text file data
/// ***
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

/// ***
/// Retrieve a constant reference to the embedded text file data
/// ***
std::string &scan::TextRc::data() const
{
    return *m_datap;
}

/// ***
/// Get a handle to the current module (executable)
/// ***
HMODULE scan::TextRc::get_module()
{
    return GetModuleHandleA(nullptr);
}

/// ***
/// Load embedded text file data from the application assembly
/// ***
void scan::TextRc::load_rc()
{
    if (m_rc_symbol == NULL)
    {
        throw LogicEx{ "TextRc::load_rc", "No resource symbol specified" };
    }

    if (!m_loaded)
    {
        const HMODULE module_handle{ get_module() };
        const char *symbolp{ MAKEINTRESOURCEA(m_rc_symbol) };

        // Locate resource info block
        HRSRC hrsrc_handle{ FindResourceA(module_handle, symbolp, &RC_TYPE[0]) };

        if (hrsrc_handle == NULL)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Failed to find resource" };
        }

        // Acquire resource handle
        HGLOBAL hglobal_handle{ LoadResource(module_handle, hrsrc_handle) };

        if (hglobal_handle == NULL)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Failed to get resource handle" };
        }

        const size_t data_size{ SizeofResource(module_handle, hrsrc_handle) };
        const char *rcp{ reinterpret_cast<char *>(LockResource(hglobal_handle)) };

        // Resource unavailable
        if (rcp == nullptr)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Requested resource unavailable" };
        }

        m_loaded = true;
        m_datap = std::make_unique<string>(string_view(rcp, data_size));
    }
}
