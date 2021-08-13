/*
*  resource.cpp
*  ------------
*  Source file for an embedded text assembly resource
*/
#include "includes/except/nullargex.h"
#include "includes/utils/resource.h"

/// ***
/// Initialize the object
/// ***
scan::Resource::Resource()
{
    m_data_ptr = nullptr;
    m_rc_handle = nullptr;
    m_mem_handle = nullptr;

    load_resource(DEFAULT_SYMBOL, DEFAULT_TYPE);
}

/// ***
/// Initialize the object
/// ***
scan::Resource::Resource(const int &t_symbol, const string &t_type)
{
    m_data_ptr = nullptr;
    m_rc_handle = nullptr;
    m_mem_handle = nullptr;

    load_resource(t_symbol, t_type);
}

/// ***
/// Cast operator overload
/// ***
scan::Resource::operator string() const
{
    return data();
}

/// ***
/// Retrieve a copy of the embedded text file data
/// ***
std::string scan::Resource::data() const
{
    string buffer;

    if (m_data_ptr != nullptr)
    {
        buffer = static_cast<string>(string_view(m_data_ptr, m_data_size));
    }
    return buffer;
}

/// ***
/// Load an embedded text resource from the project assembly
/// ***
void scan::Resource::load_resource(const int &t_symbol, const string &t_type)
{
    if (t_symbol == NULL)
    {
        throw NullArgEx{ "t_symbol" };
    }

    if (t_type.empty())
    {
        throw ArgEx("t_type", "Resource type cannot be empty");
    }

    // Locate resource info block
    m_rc_handle = FindResourceA(MODULE, MAKEINTRESOURCEA(t_symbol), t_type.c_str());

    if (m_rc_handle == NULL)
    {
        throw std::runtime_error{ "Failed to acquire resource information" };
    }

    // Get resource handle
    m_mem_handle = LoadResource(MODULE, m_rc_handle);

    if (m_mem_handle == NULL)
    {
        throw std::runtime_error{ "Failed to acquire resource handle" };
    }

    m_data_size = SizeofResource(MODULE, m_rc_handle);

    // Reinterpret any type pointer
    m_data_ptr = reinterpret_cast<char *>(LockResource(m_mem_handle));

    // Resource unavailable
    if (m_data_ptr == nullptr)
    {
        throw std::runtime_error("The requested resource is unavailable");
    }
}
