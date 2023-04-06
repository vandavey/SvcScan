/*
*  response.h
*  ----------
*  Header file for an HTTP network response message
*/
#pragma once

#ifndef RESPONSE_H
#define RESPONSE_H

#include <sdkddkver.h>
#include <boost/beast/http/parser.hpp>
#include "http_msg.h"

namespace scan
{
    /**
    * @brief  HTTP network response message.
    */
    template<HttpBody T = string_body>
    class Response final : public HttpMsg
    {
    private:  /* Type Aliases */
        using base_t = HttpMsg;
        using this_t = Response;

        using message_t = http::response<T>;

    private:  /* Fields */
        bool m_valid;       // Response is valid
        status_t m_status;  // Response status

        message_t m_resp;   // HTTP response message

    public:  /* Constructors & Destructor */
        Response();
        Response(const Response &t_response);
        Response(Response &&) = default;
        Response(const message_t &t_msg);
        Response(const string &t_raw_msg);

        virtual ~Response() = default;

    public:  /* Operators */
        Response &operator=(const Response &t_response);
        Response &operator=(Response &&) = default;

        operator std::string() const override;

        /**
        * @brief  Bitwise left shift operator overload.
        */
        inline friend ostream &operator<<(ostream &t_os, const Response &t_response)
        {
            return t_os << t_response.raw();
        }

    public:  /* Methods */
        void add_header(const header_t &t_header) override;
        void add_header(const string &t_name, const string &t_value) override;
        void parse(const message_t &t_msg);
        void parse(const string &t_raw_msg) override;
        void update_headers() override;
        void update_msg() override;

        bool ok() const noexcept;
        bool unknown() const noexcept;
        bool valid() const override;

        status_t status() const noexcept;
        uint_t status_code() const noexcept;

        const string &body() const noexcept override;
        string body(const string &t_indent) const;
        string &body(const string &t_body, const string &t_mime) override;
        string msg_header() override;
        string raw() const override;
        string raw() override;
        string reason() const;
        string server() const;
        string start_line() const override;
        string str() const override;
        string str() override;

        const message_t &message() const noexcept;
        message_t &message() noexcept;

    private:  /* Methods */
        void validate_headers() const override;
    };
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response() : base_t()
{
    m_status = status_t::unknown;
    m_valid = false;
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response(const Response &t_response)
{
    *this = t_response;
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response(const message_t &t_msg) : this_t()
{
    parse(t_msg);
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response(const string &t_raw_msg) : this_t()
{
    parse(t_raw_msg);
}

/**
* @brief  Copy assignment operator overload.
*/
template<scan::HttpBody T>
inline scan::Response<T> &scan::Response<T>::operator=(const Response &t_response)
{
    m_body = t_response.m_body;
    m_chunked = t_response.m_chunked;
    m_headers = t_response.m_headers;
    m_resp = t_response.m_resp;
    m_status = t_response.m_status;
    m_valid = t_response.m_valid;

    buffer = t_response.buffer;
    content_type = t_response.content_type;
    httpv = t_response.httpv;

    return *this;
}

/**
* @brief  Cast operator overload.
*/
template<scan::HttpBody T>
inline scan::Response<T>::operator std::string() const
{
    return this_t(*this).str();
}

/**
* @brief  Add a new HTTP header field to the underlying
*         header field map and response.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::add_header(const header_t &t_header)
{
    add_header(t_header.first, t_header.second);
}

/**
* @brief  Add a new HTTP header field to the underlying
*         header field map and response.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::add_header(const string &t_name,const string &t_value)
{
    m_headers[normalize_header(t_name)] = t_value;
    m_resp.set(normalize_header(t_name), t_value);
}

/**
* @brief  Parse information from the given HTTP response.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::parse(const message_t &t_msg)
{
    m_resp = t_msg;
    m_status = t_msg.result();
    m_valid = m_status != status_t::unknown;
    m_body = t_msg.body();

    update_msg();
}

/**
* @brief  Parse information from the given raw HTTP response.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::parse(const string &t_raw_msg)
{
    if (t_raw_msg.empty())
    {
        throw ArgEx{ "t_raw_msg", "Raw response cannot be empty" };
    }
    string raw_msg{ t_raw_msg };

    if (!raw_msg.ends_with(&CRLF[0]))
    {
        raw_msg += &CRLF[0];
    }
    size_t offset{ 0 };

    error_code ecode;
    http::response_parser<T> parser;

    do  // Parse the entire raw HTTP response
    {
        const string resp_data{ raw_msg.substr(offset) };
        const size_t bytes_read{ parser.put(asio::buffer(resp_data), ecode) };

        // Inform parser that EOF was reached
        if (bytes_read <= 0)
        {
            error_code put_eof_ecode;
            parser.put_eof(put_eof_ecode);
        }
        offset += bytes_read;
    }
    while (!parser.is_done());

    m_valid = parser.get().result() != status_t::unknown;
    parse(parser.get());
}

/**
* @brief  Synchronize the underlying response header fields and member header fields.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::update_headers()
{
    // Add 'Content-Type' header
    if (m_body.size() > 0)
    {
        if (content_type.empty())
        {
            content_type = mime_type("text", "plain");
        }
        add_header("Content-Type", content_type);
    }

    // Update response using member headers
    for (const header_t &header : m_headers)
    {
        m_resp.set(header.first, header.second);
    }
    add_headers(m_resp.base());
}

/**
* @brief  Update the underlying HTTP response using the current member values.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::update_msg()
{
    update_headers();

    m_resp.body() = m_body;
    m_resp.prepare_payload();
    m_resp.result(m_status);

    update_headers();
}

/**
* @brief  Determine whether the underlying HTTP response status code is 200 (OK).
*/
template<scan::HttpBody T>
inline bool scan::Response<T>::ok() const noexcept
{
    return status() == status_t::ok;
}

/**
* @brief  Determine whether the underlying HTTP response status code is unknown.
*/
template<scan::HttpBody T>
inline bool scan::Response<T>::unknown() const noexcept
{
    return status() == status_t::unknown;
}

/**
* @brief  Determine whether the underlying HTTP response message is valid.
*/
template<scan::HttpBody T>
inline bool scan::Response<T>::valid() const
{
    return m_valid;
}

/**
* @brief  Get the underlying HTTP response status code as an enumeration type.
*/
template<scan::HttpBody T>
inline scan::http::status scan::Response<T>::status() const noexcept
{
    return m_status;
}

/**
* @brief  Get the underlying HTTP response status code as an unsigned integer.
*/
template<scan::HttpBody T>
inline unsigned int scan::Response<T>::status_code() const noexcept
{
    return static_cast<uint_t>(m_status);
}

/**
* @brief  Get a constant reference to the underlying HTTP message body.
*/
template<scan::HttpBody T>
inline const std::string &scan::Response<T>::body() const noexcept
{
    return m_body;
}

/**
* @brief  Get the underlying HTTP message body with each line
*         indented using the specified indent string.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::body(const string &t_indent) const
{
    sstream stream;
    const string body_buffer{ algo::replace(m_body, &CRLF[0], &LF[0]) };

    for (const string &line : algo::split(m_body, &LF[0]))
    {
        stream << algo::concat(t_indent, line, &LF[0]);
    }
    return stream.str();
}

/**
* @brief  Set the underlying HTTP response body value.
*/
template<scan::HttpBody T>
inline std::string &scan::Response<T>::body(const string &t_body,
                                            const string &t_mime) {
    m_body = t_body;
    content_type = t_mime;

    update_msg();

    return m_body;
}

/**
* @brief  Get the underlying HTTP response header as a string.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::msg_header()
{
    return algo::to_string(m_resp.base());
}

/**
* @brief  Get the underlying HTTP response as a string. Chunked
*         transfer-encoding chunk sizes will be included.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::raw() const
{
    return this_t(*this).raw();
}

/**
* @brief  Get the underlying HTTP response as a string. Chunked
*         transfer-encoding chunk sizes will be included.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::raw()
{
    update_msg();
    return algo::to_string(m_resp);
}

/**
* @brief  Get the response phrase of the underlying HTTP response.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::reason() const
{
    string reason_str;

    if (!unknown())
    {
        reason_str = algo::to_string(status());
    }
    return reason_str;
}

/**
* @brief  Get the value of the underlying 'Server' HTTP header field.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::server() const
{
    string server_str;

    if (contains_header("Server"))
    {
        server_str = m_headers.at("Server");
    }
    return server_str;
}

/**
* @brief  Get the start-line of the underlying HTTP response header.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::start_line() const
{
    return algo::fstr("% % %", httpv, status_code(), reason());
}

/**
* @brief  Get the underlying HTTP response as a string. Chunked
*         transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::str() const
{
    return this_t(*this).str();
}

/**
* @brief  Get the underlying HTTP response as a string. Chunked
*         transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::str()
{
    sstream stream;

    update_msg();
    stream << m_resp.base() << m_resp.body();

    string resp_str{ stream.str() };

    if (unknown())
    {
        resp_str = algo::erase(resp_str, "<unknown-status>");
    }
    return resp_str;
}

/**
* @brief  Get a constant reference to the underlying HTTP response message.
*/
template<scan::HttpBody T>
inline const scan::http::response<T> &scan::Response<T>::message() const noexcept
{
    return m_resp;
}

/**
* @brief  Get a constant reference to the underlying HTTP response message.
*/
template<scan::HttpBody T>
inline scan::http::response<T> &scan::Response<T>::message() noexcept
{
    return m_resp;
}

/**
* @brief  Validate the HTTP header entries in the underlying header field map.
*         Throws a runtime exception when validation fails.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::validate_headers() const
{
    const string caller{ "Response<T>::validate_headers" };

    if (m_headers.empty())
    {
        throw RuntimeEx{ caller, "Underlying header map cannot be empty" };
    }
    const header_map::const_iterator server_it{ m_headers.find("Server") };

    // Missing 'Server' header key
    if (server_it == m_headers.end())
    {
        throw RuntimeEx{ caller, "Missing required header 'Server'" };
    }

    // Missing 'Server' header value
    if (server_it->second.empty())
    {
        throw RuntimeEx{ caller, "Missing value for required header 'Server'" };
    }
}

#endif // !RESPONSE_H
