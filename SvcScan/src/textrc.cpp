/*
*  textrc.h
*  --------
*  Source file for an embedded text file resource
*/
#include "includes/except/logicex.h"
#include "includes/rc/textrc.h"

/// ***
/// Initialize the object
/// ***
scan::TextRc::TextRc(const symbol &t_symbol) : base(t_symbol)
{
    m_rc_ptr = nullptr;
    load();
}

/// ***
/// Retrieve a copy of the embedded text file data
/// ***
std::string scan::TextRc::data() const
{
    string buffer;

    if (m_rc_ptr != nullptr)
    {
        buffer = static_cast<string>(string_view(m_rc_ptr, m_data_size));
    }
    return buffer;
}

/// ***
/// Load embedded text file data from the application assembly
/// ***
void scan::TextRc::load()
{
    if (m_rc_symbol == NULL)
    {
        throw LogicEx("TextRc::load", "No resource symbol specified");
    }

    const HMODULE module_handle{ get_module() };
    const char *symbol_ptr{ MAKEINTRESOURCEA(m_rc_symbol) };

    // Locate resource info block
    m_rc_handle = FindResourceA(module_handle, symbol_ptr, &RC_TYPE[0]);

    if (m_rc_handle == NULL)
    {
        throw std::runtime_error{ "Failed to acquire resource information" };
    }

    // Get resource handle
    m_mem_handle = LoadResource(module_handle, m_rc_handle);

    if (m_mem_handle == NULL)
    {
        throw std::runtime_error{ "Failed to acquire resource handle" };
    }

    m_data_size = SizeofResource(module_handle, m_rc_handle);
    m_rc_ptr = reinterpret_cast<char *>(LockResource(m_mem_handle));

    // Resource unavailable
    if (m_rc_ptr == nullptr)
    {
        throw std::runtime_error("The requested resource could not be acquired");
    }
}
