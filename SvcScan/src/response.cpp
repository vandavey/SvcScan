/*
*  response.cpp
*  ------------
*  Source file for a HTTP network response
*/
#include "includes/inet/http/response.h"
#include "includes/utils/util.h"

/// ***
/// Initialize the object
/// ***
scan::Response::Response(const Response &t_response) : base()
{
    operator=(t_response);
}

/// ***
/// Initialize the object
/// ***
scan::Response::Response(const string &t_raw_resp) : base()
{
    operator=(t_raw_resp);
}

/// ***
/// Assignment operator overload
/// ***
scan::Response &scan::Response::operator=(const Response &t_response)
{
    m_content_len = t_response.m_content_len;
    m_payload = t_response.m_payload;
    m_headers = t_response.m_headers;

    code = t_response.code;
    content_type = t_response.content_type;
    date = t_response.date;
    server = t_response.server;
    status = t_response.status;
    version = t_response.version;

    // Update underlying header map
    update_headers();

    return *this;
}

/// ***
/// Assignment operator overload
/// ***
scan::Response &scan::Response::operator=(const string &t_raw_resp)
{
    return parse(t_raw_resp);
}

/// ***
/// Cast operator overload
/// ***
scan::Response::operator string() const
{
    Response clone{ *this };
    clone.update_headers();

    return clone.raw();
}

/// ***
/// Bitwise left shift operator overload
/// ***
std::ostream &scan::operator<<(std::ostream &t_os, const Response &t_response)
{
    return (t_os << static_cast<std::string>(t_response));
}

/// ***
/// Convert the current HTTP response to a raw string
/// ***
std::string scan::Response::raw() const
{
    std::stringstream ss;
    const string raw_headers{ header_str() };

    ss << version << " "  << code << " " << status << CRLF
        << raw_headers << CRLF << CRLF;

    // Add HTTP message payload
    if (!m_payload.empty())
    {
        ss << m_payload << CRLF;
    }
    return ss.str();
}

/// ***
/// Update member field values using the underlying header map
/// ***
void scan::Response::update_members()
{
    if (!m_headers.empty())
    {
        // Get 'Content-Length' header value
        if (contains_header("Content-Length"))
        {
            m_content_len = std::stoi(m_headers["Content-Length"]);
        }

        // Get 'Content-Type' header value
        if (contains_header("Content-Type"))
        {
            content_type = m_headers["Content-Type"];
        }

        // Get 'Date' header value
        if (contains_header("Date"))
        {
            date = m_headers["Date"];
        }

        // Get 'Server' header value
        if (contains_header("Server"))
        {
            server = m_headers["Server"];
        }
    }
}

/// ***
/// Parse the given raw HTTP data to form a response object
/// ***
std::string scan::Response::parse_payload(const string &t_raw_payload,
                                          const size_t &t_content_len) {
    if (!t_raw_payload.empty())
    {
        m_payload = std::string_view(t_raw_payload.c_str(), t_content_len);
    }
    return m_payload;
}

/// ***
/// Parse the given raw HTTP data to form a response object
/// ***
scan::Response &scan::Response::parse(const string &t_raw_resp)
{
    if (t_raw_resp.empty())
    {
        throw ArgEx{ "t_raw_resp", "The given raw HTTP data cannot be empty" };
    }

    // Split HTTP metadata and payload data
    const vector_s msg_sections{ Util::split(t_raw_resp, Util::fstr("%%", CRLF)) };

    // Unparsable HTTP data
    if (msg_sections.empty())
    {
        throw ArgEx{ "t_raw_resp", "The given raw HTTP data is unparsable" };
    }

    const string info_line{ t_raw_resp.substr(0, t_raw_resp.find(CRLF)) };
    const vector_s http_info{ Util::split(info_line, " ", 2) };

    // Unparsable HTTP data
    if (http_info.size() != 3)
    {
        throw ArgEx{ "t_raw_resp", "The given raw HTTP data is unparsable" };
    }

    version = http_info[0].substr(http_info[0].rfind("/") + 1);
    code = static_cast<uint>(std::stoi(http_info[1]));
    status = http_info[2];

    // Parse raw header data
    parse_headers(msg_sections[0]);

    // Update member values from headers
    update_members();

    // Parse raw payload data
    if (msg_sections.size() == 2)
    {
        parse_payload(msg_sections[1], m_content_len);
    }
    return *this;
}

/// ***
/// Parse the given raw HTTP message headers (metadata)
/// ***
scan::Response::header_map scan::Response::parse_headers(const string &t_raw_headers)
{
    if (t_raw_headers.empty())
    {
        throw ArgEx{ "t_raw_headers", "Raw HTTP headers cannot be empty" };
    }

    // Add headers to underlying dictionary
    if (!t_raw_headers.empty())
    {
        for (const string &header_str : Util::split(t_raw_headers, CRLF))
        {
            if (header_str.find(":") != string::npos)
            {
                const vector_s pair{ Util::split(header_str, ":", 1) };
                add_header(Util::rstrip(pair[0]), Util::lstrip(pair[1]));
            }
        }
    }
    return m_headers;
}

/// ***
/// Update the underlying header map with the current member values
/// ***
scan::Response::header_map scan::Response::update_headers()
{
    m_content_len = m_payload.size();

    return add_headers(
    {
        { "Connection",     CONNECTION },
        { "Content-Length", std::to_string(m_content_len) },
        { "Content-Type",   content_type },
        { "Date",           date },
        { "Server",         server }
    });
}
