/*
*  text_rc.h
*  ---------
*  Source file for an embedded text file resource
*/
#include "includes/errors/logic_ex.h"
#include "includes/errors/runtime_ex.h"
#include "includes/resources/text_rc.h"

/**
* @brief  Initialize the object.
*/
scan::TextRc::TextRc() noexcept
{
    m_loaded = false;
    m_rc_symbol = INVALID_SYMBOL;
}

/**
* @brief  Initialize the object.
*/
scan::TextRc::TextRc(TextRc &&t_trc) noexcept
{
    *this = std::forward<this_t>(t_trc);
}

/**
* @brief  Initialize the object.
*/
scan::TextRc::TextRc(const symbol_t &t_symbol) : this_t()
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
* @brief  Get a line from the underlying text data at the specified line index.
*         Returns true if the line data was successfully copied.
*/
bool scan::TextRc::get_line(string &t_ln_buffer, const size_t &t_ln_idx) const
{
    if (!m_loaded)
    {
        throw LogicEx{ "TextRc::get_line", "Resource must be loaded" };
    }
    bool ln_found{ false };

    if (t_ln_idx < algo::count(*m_datap, &LF[0]))
    {
        RangeIterator auto beg{ algo::find_nth(*m_datap, &LF[0], t_ln_idx, true) };
        RangeIterator auto end{ algo::find_nth(*m_datap, &LF[0], t_ln_idx + 1) };

        // Error occurred while searching string data
        if (beg == m_datap->end() || end == m_datap->end())
        {
            throw RuntimeEx{ "TextRc::get_line", "Error occurred finding line" };
        }

        t_ln_buffer = algo::substr(*m_datap, beg, end);
        ln_found = true;
    }
    return ln_found;
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
    if (m_rc_symbol == INVALID_SYMBOL)
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

        const ulong_t data_size{ SizeofResource(module_handle, hrsrc_handle) };
        const char *rcp{ static_cast<char *>(LockResource(hglobal_handle)) };

        // Resource is unavailable
        if (rcp == nullptr)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Requested resource unavailable" };
        }

        m_loaded = true;
        m_datap = std::make_unique<string>(string_view(rcp, data_size));
    }
}
