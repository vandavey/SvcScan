/*
* @file
*     message.h
* @brief
*     Header file for a virtual HTTP message.
*/
#pragma once

#ifndef SCAN_MESSAGE_H
#define SCAN_MESSAGE_H

#include <cctype>
#include <string>
#include <boost/beast/http/fields.hpp>
#include "../../concepts/http_concepts.h"
#include "../../contracts/i_string_castable.h"
#include "../../ranges/algo.h"
#include "../../ranges/list.h"
#include "../../utils/aliases.h"
#include "../../utils/const_defs.h"
#include "../../utils/literals.h"
#include "../net_aliases.h"
#include "../net_const_defs.h"
#include "http_version.h"

namespace scan
{
    /**
    * @brief
    *     Virtual HTTP network message.
    */
    template<HttpMessage T>
    class Message : public IStringCastable
    {
    protected:  /* Type Aliases */
        using buffer_t  = flat_buffer;
        using field_t   = http::fields::value_type;
        using fields    = http::fields;
        using message_t = T;

    public:  /* Fields */
        HttpVersion httpv;  // HTTP version
        buffer_t buffer;    // Message buffer

    protected:  /* Fields */
        string m_body;          // Message body
        string m_content_type;  // 'Content-Type' header

        header_map m_headers;   // HTTP header fields
        message_t m_msg;        // HTTP message

    public:  /* Constructors & Destructor */
        Message() = default;
        Message(const Message&) = default;
        Message(Message&&) = default;

        virtual ~Message() = default;

    public:  /* Operators */
        Message& operator=(const Message&) = default;
        Message& operator=(Message&&) = default;

        operator std::string() const override;

    public:  /* Methods */
        /**
        * @brief
        *     Get the HTTP MIME type with the 'charset' parameter set.
        */
        static constexpr string mime_type(const string& t_type,
                                          const string& t_subtype = MIME_TYPE_WILDCARD,
                                          const string& t_charset = CHARSET_UTF8)
        {
            return algo::fstr("%/%;charset=%", t_type, t_subtype, t_charset);
        }

        /**
        * @brief
        *     Get a constant reference to the underlying HTTP message body.
        */
        virtual constexpr const string& body() const noexcept
        {
            return this->m_body;
        }

        /**
        * @brief
        *     Get a copy of the underlying HTTP message body with
        *     each line indented using the specified indent string.
        */
        virtual constexpr string body(const string& t_indent) const
        {
            const string normalized_body{algo::replace(this->m_body, CRLF, LF)};
            const List<string> lines{algo::split(normalized_body, LF)};

            sstream stream;

            for (const string& line : lines)
            {
                stream << t_indent << line;

                if (&line != &lines.last())
                {
                    stream << LF;
                }
            }
            return stream.str();
        }

        /**
        * @brief
        *     Get a copy of the underlying HTTP message header field map.
        */
        constexpr header_map msg_headers() const noexcept
        {
            return m_headers;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying HTTP message.
        */
        constexpr const T& message() const noexcept
        {
            return m_msg;
        }

        /**
        * @brief
        *     Get a reference to the underlying HTTP message.
        */
        constexpr T& message() noexcept
        {
            return m_msg;
        }

        void add_header(const header_t& t_header);
        void add_header(const string& t_name, const string& t_value);
        void add_headers(const header_map& t_headers);
        void body(const string& t_body, const string& t_mime = {});
        virtual void update_msg() = 0;

        bool contains(const string& t_name) const;
        virtual bool valid() const = 0;

        size_t content_length() const;

        string msg_header();
        string raw() const;
        string raw_headers(const string& t_indent = {}) const;
        virtual string start_line() const = 0;
        string str() const;
        string str();

    protected:  /* Methods */
        static string normalize_header(const string& t_name);

        static header_map make_header_map(const string& t_raw_headers);
        static header_map make_header_map(const fields& t_fields);

        void add_headers(const string& t_raw_headers);
        void add_headers(const fields& t_fields);
        void update_content_type();
        void update_member_headers();
        void update_message_headers();
        virtual void validate_headers() const = 0;
    };
}

/**
* @brief
*     Cast operator overload.
*/
template<scan::HttpMessage T>
inline scan::Message<T>::operator std::string() const
{
    return str();
}

/**
* @brief
*     Add a new HTTP header field to the underlying header field map and message.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::add_header(const header_t& t_header)
{
    add_header(t_header.first, t_header.second);
}

/**
* @brief
*     Add a new HTTP header field to the underlying header field map and message.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::add_header(const string& t_name, const string& t_value)
{
    m_headers[normalize_header(t_name)] = t_value;
    m_msg.set(normalize_header(t_name), t_value);
}

/**
* @brief
*     Add the given HTTP header fields to the underlying header field map and message.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::add_headers(const header_map& t_headers)
{
    for (const header_t& header : t_headers)
    {
        add_header(header);
    }
}

/**
* @brief
*     Set the underlying HTTP message body value and 'Content-Type' header field.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::body(const string& t_body, const string& t_mime)
{
    m_body = t_body;
    m_content_type = t_mime;

    update_msg();
}

/**
* @brief
*     Determine whether the underlying header field map contains the given header field.
*/
template<scan::HttpMessage T>
inline bool scan::Message<T>::contains(const string& t_name) const
{
    return m_headers.contains(normalize_header(t_name));
}

/**
* @brief
*     Get the 'Content-Length' header field value from the underlying header field map.
*/
template<scan::HttpMessage T>
inline size_t scan::Message<T>::content_length() const
{
    size_t length{0_st};

    if (contains(HTTP_CONTENT_LENGTH))
    {
        length = algo::to_uint(m_headers.at(HTTP_CONTENT_LENGTH));
    }
    return length;
}

/**
* @brief
*     Get the underlying HTTP message header as a string.
*/
template<scan::HttpMessage T>
inline std::string scan::Message<T>::msg_header()
{
    return algo::to_string(m_msg.base());
}

/**
* @brief
*     Get the underlying HTTP message as a string. Chunked
*     transfer-encoding chunk sizes will be included.
*/
template<scan::HttpMessage T>
inline std::string scan::Message<T>::raw() const
{
    return algo::to_string(m_msg);
}

/**
* @brief
*     Get the underlying HTTP message header fields in their raw form.
*/
template<scan::HttpMessage T>
inline std::string scan::Message<T>::raw_headers(const string& t_indent) const
{
    sstream stream;

    for (size_t i{0_st}; const header_t& header : m_headers)
    {
        stream << algo::fstr("%%: %", t_indent, header.first, header.second);

        if (i++ != m_headers.size() - 1)
        {
            stream << CRLF;
        }
    }
    return stream.str();
}

/**
* @brief
*     Get the underlying HTTP message as a string. Chunked
*     transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpMessage T>
inline std::string scan::Message<T>::str() const
{
    sstream stream;
    stream << m_msg.base() << m_msg.body();

    return stream.str();
}

/**
* @brief
*     Get the underlying HTTP message as a string. Chunked
*     transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpMessage T>
inline std::string scan::Message<T>::str()
{
    sstream stream;

    update_msg();
    stream << m_msg.base() << m_msg.body();

    return stream.str();
}

/**
* @brief
*     Normalize the casing of the given HTTP message header field
*     name to avoid parsing errors caused by duplicate header checks.
*/
template<scan::HttpMessage T>
inline std::string scan::Message<T>::normalize_header(const string& t_name)
{
    string header_name;

    if (!t_name.empty())
    {
        vector<string> new_parts;

        // Normalize header name casing
        for (const string& header_part : algo::split(t_name, "-"))
        {
            string part{algo::to_lower(header_part)};

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
* @brief
*     Create a new header field map from the given raw HTTP message header fields.
*/
template<scan::HttpMessage T>
inline scan::header_map scan::Message<T>::make_header_map(const string& t_raw_headers)
{
    header_map headers;

    if (!t_raw_headers.empty())
    {
        for (const string& raw_header : algo::split(t_raw_headers, CRLF))
        {
            if (algo::contains(raw_header, ":"))
            {
                const string_array<2> kv_pair{algo::split<2>(raw_header, ":")};
                const string name{normalize_header(algo::trim_right(kv_pair[0]))};

                headers[name] = algo::trim_left(kv_pair[1]);
            }
        }
    }
    return headers;
}

/**
* @brief
*     Create a new header field map from the given HTTP message headers fields.
*/
template<scan::HttpMessage T>
inline scan::header_map scan::Message<T>::make_header_map(const fields& t_fields)
{
    header_map headers;

    for (const field_t& field : t_fields)
    {
        headers[normalize_header(field.name_string())] = field.value();
    }
    return headers;
}

/**
* @brief
*     Parse the HTTP message header fields from the given raw headers
*     and add them to the underlying header field map and message.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::add_headers(const string& t_raw_headers)
{
    add_headers(make_header_map(t_raw_headers));
}

/**
* @brief
*     Add the given HTTP message header fields to
*     the underlying header field map and message.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::add_headers(const fields& t_fields)
{
    add_headers(make_header_map(t_fields));
}

/**
* @brief
*     Update the 'Content-Type' header field in
*     the underlying header field map and message.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::update_content_type()
{
    if (m_body.size() > 0)
    {
        if (m_content_type.empty())
        {
            m_content_type = mime_type(MIME_TYPE_TEXT, MIME_SUBTYPE_PLAIN);
        }
        add_header(HTTP_CONTENT_TYPE, m_content_type);
    }
}

/**
* @brief
*     Update the underlying HTTP header field map member
*     using the current values of the HTTP message member.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::update_member_headers()
{
    add_headers(m_msg.base());
}

/**
* @brief
*     Update the underlying HTTP message member using the
*     current values of the HTTP header field map member.
*/
template<scan::HttpMessage T>
inline void scan::Message<T>::update_message_headers()
{
    for (const header_t& header : m_headers)
    {
        m_msg.set(header.first, header.second);
    }
}

#endif // !SCAN_MESSAGE_H
