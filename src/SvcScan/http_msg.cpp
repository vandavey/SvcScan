/*
*  http_msg.cpp
*  ------------
*  Source file for an abstract HTTP message
*/
#include "includes/errors/runtime_ex.h"
#include "includes/inet/http/http_msg.h"

/**
* @brief  Initialize the object.
*/
scan::HttpMsg::HttpMsg()
{
    m_chunked = false;
}

/**
* @brief  Initialize the object.
*/
scan::HttpMsg::HttpMsg(const HttpMsg &t_msg)
{
    m_body = t_msg.m_body;
    m_chunked = t_msg.m_chunked;
    m_headers = t_msg.m_headers;

    buffer = t_msg.buffer;
    content_type = t_msg.content_type;
    httpv = t_msg.httpv;
}

/**
* @brief  Get the HTTP MIME type with the 'charset' parameter set.
*/
std::string scan::HttpMsg::mime_type(const string &t_type, const string &t_subtype)
{
    return algo::fstr("%/%;charset=%", t_type, t_subtype, &CHARSET[0]);
}

/**
* @brief  Add a new HTTP header field to the underlying header field map.
*/
void scan::HttpMsg::add_header(const header_t &t_header)
{
    add_header(t_header.first, t_header.second);
}

/**
* @brief  Add a new HTTP header field to the underlying header field map.
*/
void scan::HttpMsg::add_header(const string &t_name, const string &t_value)
{
    m_headers[normalize_header(t_name)] = t_value;
}

/**
* @brief  Add the given HTTP header fields to the underlying header field map.
*/
void scan::HttpMsg::add_headers(const header_map &t_headers)
{
    for (const header_t &header : t_headers)
    {
        add_header(header);
    }
}

/**
* @brief  Determine whether the underlying header field map contains
*         the given header field.
*/
bool scan::HttpMsg::contains_header(const string &t_name) const
{
    bool found{ false };

    for (const header_t &header : m_headers)
    {
        if (found = header.first == normalize_header(t_name))
        {
            break;
        }
    }
    return found;
}

/**
* @brief  Determine whether the underlying HTTP message is
*         using chunked transfer-encoding.
*/
bool scan::HttpMsg::is_chunked() const noexcept
{
    return m_chunked;
}

/**
* @brief  Get the 'Content-Length' header field value from the
*         underlying header field map.
*/
size_t scan::HttpMsg::content_length() const
{
    size_t length{ 0 };

    if (contains_header("Content-Length"))
    {
        length = algo::to_uint(m_headers.at("Content-Length"));
    }
    return length;
}

/**
* @brief  Get the underlying HTTP message header fields in their raw form.
*/
std::string scan::HttpMsg::raw_headers(const string &t_indent) const
{
    sstream stream;

    for (size_t i{ 0 }; const header_t &header : m_headers)
    {
        stream << algo::fstr("%%: %", t_indent, header.first, header.second);

        if (i++ != m_headers.size() - 1)
        {
            stream << &CRLF[0];
        }
    }
    return stream.str();
}

/**
* @brief  Get a copy of the underlying HTTP message header field map.
*/
scan::header_map scan::HttpMsg::msg_headers() const noexcept
{
    return m_headers;
}

/**
* @brief  Normalize the casing of the given HTTP message header field name to
*         avoid parsing errors caused by duplicate header checks.
*/
std::string scan::HttpMsg::normalize_header(const string &t_name)
{
    string header_name;

    if (!t_name.empty())
    {
        string_vector new_parts;

        // Normalize header name casing
        for (const string &header_part : algo::split(t_name, "-"))
        {
            string part{ algo::to_lower(header_part) };

            if (!part.empty())
            {
                part[0] = std::toupper(part[0]);
            }
            new_parts.push_back(part);
        }
        header_name = algo::join(new_parts, "-");
    }
    return header_name;
}

/**
* @brief  Create a new header field map from the given raw HTTP message header fields.
*/
scan::header_map scan::HttpMsg::map(const string &t_raw_headers)
{
    header_map headers;

    if (!t_raw_headers.empty())
    {
        for (const string &raw_header : algo::split(t_raw_headers, &CRLF[0]))
        {
            if (raw_header.find(":") != string::npos)
            {
                const string_array<2> kv_pair{ algo::split<2>(raw_header, ":") };
                const string name{ normalize_header(algo::trim_right(kv_pair[0])) };

                headers[name] = algo::trim_left(kv_pair[1]);
            }
        }
    }
    return headers;
}

/**
* @brief  Create a new header field map from the given HTTP message headers fields.
*/
scan::header_map scan::HttpMsg::map(const fields &t_fields)
{
    header_map headers;

    for (const field_t &field : t_fields)
    {
        const string name{ field.name_string() };
        const string value{ field.value() };

        headers[normalize_header(name)] = value;
    }
    return headers;
}

/**
* @brief  Parse the HTTP message header fields from the given raw headers and
*         add them to the underlying header field map.
*/
void scan::HttpMsg::add_headers(const string &t_raw_headers)
{
    add_headers(map(t_raw_headers));
}

/**
* @brief  Add the given HTTP message header fields to the
*         underlying header field map.
*/
void scan::HttpMsg::add_headers(const fields &t_fields)
{
    add_headers(map(t_fields));
}
