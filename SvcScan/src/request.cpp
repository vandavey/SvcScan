/*
*  request.cpp
*  -----------
*  Source file for a HTTP network request
*/
#include "includes/inet/http/request.h"

/// ***
/// Initialize the object
/// ***
scan::Request::Request(const Request &t_request) : base()
{
    accept = mime_type("text");
    operator=(t_request);
}

/// ***
/// Initialize the object
/// ***
scan::Request::Request(const string &t_method,
                       const string &t_host,
                       const string &t_payload,
                       const string &t_uri) : base(t_payload) {

    m_method = Util::to_upper(t_method);

    accept = mime_type("text");
    host = t_host;
    uri = t_uri;
}

/// ***
/// Assignment operator overload
/// ***
scan::Request &scan::Request::operator=(const Request &t_request)
{
    m_method = t_request.m_method;
    m_content_len = t_request.m_content_len;
    m_payload = t_request.m_payload;
    m_headers = t_request.m_headers;

    accept = t_request.accept;
    content_type = t_request.content_type;
    host = t_request.host;
    referer = t_request.referer;
    user_agent = t_request.user_agent;
    version = t_request.version;
    uri = t_request.uri;

    // Update underlying header map
    update_headers();

    return *this;
}

/// ***
/// Cast operator overload
/// ***
scan::Request::operator string() const
{
    Request clone{ *this };
    clone.update_headers();

    return clone.raw();
}

/// ***
/// Bitwise left shift operator overload
/// ***
std::ostream &scan::operator<<(std::ostream &t_os, const Request &t_request)
{
    return (t_os << static_cast<std::string>(t_request));
}

/// ***
/// Set the string payload value to send in the HTTP message
/// ***
std::string scan::Request::payload(const string &t_payload, const string &t_mime)
{
    if (t_payload.empty())
    {
        throw ArgEx("t_payload", "Payload cannot be empty");
    }

    m_content_len = t_payload.size();
    m_method = POST;
    m_payload = t_payload;
    content_type = t_mime;

    // Update header dictionary
    update_headers();

    return m_payload;
}

/// ***
/// Convert the current HTTP request to a raw string
/// ***
std::string scan::Request::raw() const
{
    std::stringstream ss;
    const string raw_headers{ header_str() };

    ss << m_method << " " << uri << " " << HTTP_VERSION << stdu::CRLF
        << raw_headers << stdu::CRLF << stdu::CRLF;

    // Add HTTP message payload
    if (!m_payload.empty())
    {
        ss << m_payload << stdu::CRLF;
    }
    return ss.str();
}

/// ***
/// Update the underlying header map with the current member values
/// ***
scan::Request::header_map scan::Request::update_headers()
{
    m_content_len = m_payload.size();

    return add_headers(
    {
        { "Host",           host },
        { "Accept",         accept },
        { "Connection",     CONNECTION },
        { "Content-Length", std::to_string(m_content_len) },
        { "Content-Type",   content_type },
        { "User-Agent",     user_agent }
    });
}
