/*
*  httpmsg.cpp
*  -----------
*  Source file for a HTTP message abstract base class
*/
#include <sstream>
#include "includes/inet/http/httpmsg.h"

/// ***
/// Initialize the object
/// ***
scan::HttpMsg::HttpMsg(const HttpMsg &t_msg)
{
    operator=(t_msg);
}

/// ***
/// Initialize the object
/// ***
scan::HttpMsg::HttpMsg(const string &t_payload, const string &t_mime) : HttpMsg()
{
    if (!t_payload.empty())
    {
        payload(t_payload, t_mime);
    }
}

/// ***
/// Initialize the object
/// ***
scan::HttpMsg::HttpMsg(const header_map &t_headers,
                       const string &t_payload,
                       const string &t_mime) : HttpMsg(t_payload, t_mime) {

    add_headers(t_headers);
}

/// ***
/// Initialize the object
/// ***
scan::HttpMsg::HttpMsg()
{
    m_content_len = 0;
    version = HTTP_VERSION;

    add_headers(default_headers());
}

/// ***
/// Assignment operator overload
/// ***
scan::HttpMsg &scan::HttpMsg::operator=(const HttpMsg &t_msg) noexcept
{
    m_content_len = t_msg.m_content_len;
    m_payload = t_msg.m_payload;
    m_headers = t_msg.m_headers;

    content_type = t_msg.content_type;
    version = t_msg.version;

    return *this;
}

/// ***
/// Bitwise left shift operator overload
/// ***
std::ostream &scan::operator<<(std::ostream &t_os, const HttpMsg &t_msg)
{
    return (t_os << static_cast<std::string>(t_msg));
}

/// ***
/// Add a new HTTP header to the underlying header map
/// ***
void scan::HttpMsg::add_header(const header &t_header)
{
    m_headers[normalize_header(t_header.first)] = t_header.second;
}

/// ***
/// Add a new HTTP header to the underlying header map
/// ***
void scan::HttpMsg::add_header(const string &t_key, const string &t_val)
{
    add_header(header{ t_key, t_val });
}

/// ***
/// Determine whether the underlying header map contains the given header key
/// ***
bool scan::HttpMsg::contains_header(const string &t_key,
                                    const bool &t_not_blank) const {

    const header_map::const_iterator const_it{ m_headers.find(t_key) };
    const bool key_found{ const_it != m_headers.end() };

    return t_not_blank ? (key_found && !const_it->second.empty()) : key_found;
}

/// ***
/// Retrieve the underlying HTTP string payload
/// ***
std::string scan::HttpMsg::get_payload() const noexcept
{
    return m_payload;
}

/// ***
/// Set the string payload value to send in the HTTP message
/// ***
std::string scan::HttpMsg::payload(const string &t_payload, const string &t_mime)
{
    m_content_len = t_payload.size();
    content_type = t_mime;

    return (m_payload = t_payload);
}

/// ***
/// Add the given HTTP headers to the underlying header map
/// ***
scan::HttpMsg::header_map scan::HttpMsg::add_headers(const header_map &t_headers)
{
    for (const header &header : t_headers)
    {
        add_header(header);
    }
    return m_headers;
}

/// ***
/// Get the default HTTP message header map
/// ***
scan::HttpMsg::header_map scan::HttpMsg::default_headers() const
{
    return header_map
    {
        { "Connection",     CONNECTION },
        { "Content-Length", std::to_string(m_content_len) },
        { "Content-Type",   content_type }
    };
}

/// ***
/// Retrieve a copy of the underlying header map
/// ***
scan::HttpMsg::header_map scan::HttpMsg::get_headers() const noexcept
{
    return m_headers;
}

/// ***
/// Get the HTTP MIME type with the 'charset' parameter set
/// ***
std::string scan::HttpMsg::mime_type(const string &t_type, const string &t_subtype)
{
    std::stringstream ss;
    ss << t_type << "/" << t_subtype << "; charset=" << CHARSET;

    return ss.str();
}

/// ***
/// Normalize the casing of the given header key to avoid duplicates
/// ***
std::string scan::HttpMsg::normalize_header(const string &t_key)
{
    string header_key;

    // Skip normalization of blank header keys
    if (!t_key.empty())
    {
        vector_s new_parts;

        // Normalize header casing
        for (const string &header_part : Util::split(t_key, "-"))
        {
            string part{ Util::to_lower(header_part) };

            if (!part.empty())
            {
                part[0] = std::toupper(part[0]);
            }
            new_parts.push_back(part);
        }
        header_key = list_s::join(new_parts, "-");
    }
    return header_key;
}

/// ***
/// Validate the HTTP header entries in the given header map
/// ***
void scan::HttpMsg::validate_headers(const header_map &t_headers) const
{
    if (t_headers.empty())
    {
        throw ArgEx("t_headers", "The header map cannot be empty");
    }
}

/// ***
/// Join the given HTTP to send over the socket
/// ***
std::string scan::HttpMsg::raw_headers() const
{
    size_t count{ 0 };
    std::stringstream ss;

    // Add headers to the string buffer
    for (const header &header : m_headers)
    {
        if (!header.second.empty() && (header.second != "0"))
        {
            ss << header.first << ": " << header.second;

            if (count < m_headers.size() - 1)
            {
                ss << stdu::CRLF;
            }
        }
        count++;
    }
    return ss.str();
}

/// ***
/// Update the underlying header map with the current member values
/// ***
scan::HttpMsg::header_map scan::HttpMsg::update_headers()
{
    m_content_len = m_payload.size();

    return add_headers(
    {
        { "Connection",     CONNECTION },
        { "Content-Length", std::to_string(m_content_len) },
        { "Content-Type",   content_type }
    });
}
