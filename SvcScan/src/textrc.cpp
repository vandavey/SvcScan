/*
*  textrc.h
*  --------
*  Source file for an embedded text file resource
*/
#include "includes/except/logicex.h"
#include "includes/except/runtimeex.h"
#include "includes/io/stdutil.h"
#include "includes/rc/textrc.h"

/// ***
/// Initialize the object
/// ***
scan::TextRc::TextRc()
{
    m_loaded = false;
    m_rc_symbol = NULL;

    m_rc_ptr = nullptr;
    m_rc_handle = nullptr;
    m_mem_handle = nullptr;

    m_data_size = NULL;
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
scan::TextRc &scan::TextRc::operator=(const symbol_t &t_symbol)
{
    m_rc_symbol = t_symbol;
    m_loaded = false;

    load_rc();

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
    const vector<string> lines{ Util::split(data(), StdUtil::LF) };

    // Get the specified line
    if (t_line_idx < lines.size())
    {
        t_line = lines[t_line_idx];
        line_found = true;
    }
    return line_found;
}

/// ***
/// Retrieve a copy of the embedded text file data
/// ***
std::string scan::TextRc::data() const
{
    string buffer;

    if (m_loaded && (m_rc_ptr != nullptr))
    {
        buffer = string_view(m_rc_ptr, m_data_size);
    }
    return buffer;
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
        const char *symbol_ptr{ MAKEINTRESOURCEA(m_rc_symbol) };

        // Locate resource info block
        m_rc_handle = FindResourceA(module_handle, symbol_ptr, &RC_TYPE[0]);

        if (m_rc_handle == NULL)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Failed to find resource" };
        }

        // Get resource handle
        m_mem_handle = LoadResource(module_handle, m_rc_handle);

        if (m_mem_handle == NULL)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Failed to get resource handle" };
        }

        m_data_size = SizeofResource(module_handle, m_rc_handle);
        m_rc_ptr = reinterpret_cast<char *>(LockResource(m_mem_handle));

        // Resource unavailable
        if (m_rc_ptr == nullptr)
        {
            throw RuntimeEx{ "TextRc::load_rc", "Requested resource unavailable" };
        }
        m_loaded = true;
    }
}
