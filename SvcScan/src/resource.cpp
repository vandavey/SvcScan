/*
*  resource.cpp
*  ------------
*  Source file for an assembly embedded resource
*/
#include "includes/except/argex.h"
#include "includes/rc/resource.h"
#include "includes/resources.h"

/// ***
/// Initialize the object
/// ***
scan::Resource::Resource(const symbol &t_symbol) : m_rc_symbol(t_symbol)
{
    if (!valid_symbol(t_symbol))
    {
        throw ArgEx("t_symbol", "Invalid resource symbol provided");
    }

    m_rc_handle = nullptr;
    m_mem_handle = nullptr;
    m_data_size = NULL;
}

/// ***
/// Determine if the given symbol represents an embedded resource
/// ***
bool scan::Resource::valid_symbol(const symbol &t_symbol)
{
    return (t_symbol == MAINICON) || (t_symbol == CSV_DATA);
}

/// ***
/// Get a handle to the current module (executable)
/// ***
HMODULE scan::Resource::get_module()
{
    return GetModuleHandleA(NULL);
}
