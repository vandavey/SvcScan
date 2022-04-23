/*
*  httpmsg.h
*  ---------
*  Header file for a HTTP message abstract base class
*/
#pragma once

#ifndef HTTP_MSG_H
#define HTTP_MSG_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <map>
#include <string>
#include <sdkddkver.h>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/string_body.hpp>
#include "../../containers/generic/list.h"
#include "../../io/stdutil.h"
#include "httpversion.h"

namespace
{
    namespace http  = boost::beast::http;
    using field_map = std::map<std::string, std::string>;
}

namespace scan
{
    /// ***
    /// Abstract HTTP network message class
    /// ***
    template<class T>
    class HttpMsg : public IStringCastable
    {
    private:  /* Type Aliases */
        using error_code  = boost::system::error_code;
        using fields      = http::fields;
        using field_kv    = field_map::value_type;
        using field_t     = http::fields::value_type;
        using flat_buffer = boost::beast::flat_buffer;
        using stdu        = StdUtil;
        using string      = std::string;

        template<class T>
        using vector = std::vector<T>;

    protected: /* Constants */
        static constexpr char CHARSET[]    = "UTF-8";  // MIME character set
        static constexpr char CONNECTION[] = "close";  // Default 'Connection' header
        static constexpr char WILDCARD[]   = "*";      // MIME wild card

    public:  /* Fields */
        HttpVersion httpv;    // HTTP version

        string content_type;  // 'Content-Type' header
        flat_buffer buffer;   // Message buffer

    protected:  /* Fields */
        string m_body;       // Message body
        field_map m_fields;  // HTTP header fields

    public:  /* Constructors & Destructor */
        HttpMsg();
        HttpMsg(const HttpMsg &t_msg);
        HttpMsg(const string &t_body, const string &t_mime = string());
        HttpMsg(const field_map &t_fields);

        HttpMsg(const field_map &t_fields,
                const string &t_body = string(),
                const string &t_mime = string());

        virtual ~HttpMsg() = default;

    private:  /* Assertions */
        static_assert(std::is_same_v<T, http::string_body>
                      || std::is_same_v<T, http::dynamic_body>,
                      "Class 'T' must be must be 'dynamic_body' or 'string_body'");

    public:  /* Methods */
        static string mime_type(const string &t_type,
                                const string &t_subtype = WILDCARD);

        virtual void add_field(const field_kv &t_field_kvp);
        virtual void add_field(const string &t_key, const string &t_val);
        void add_fields(const field_map &t_fields);
        virtual void parse(const string &t_raw_msg) = 0;
        virtual void update_msg() = 0;

        bool contains_field(const string &t_field) const;
        virtual bool valid() const = 0;

        size_t content_length() const;

        virtual string body() const noexcept;
        virtual string body(const string &t_body, const string &t_mime);
        virtual string msg_header() = 0;
        string raw_fields() const;
        virtual string start_line() const = 0;

        virtual field_map default_fields() const;
        field_map msg_fields() const noexcept;

    protected:  /* Methods */
        static string normalize_field(const string &t_key);

        static field_map map(const string &t_raw_fields);
        static field_map map(const fields &t_msg_header);

        void add_fields(const string &t_raw_fields);
        void add_fields(const fields &t_msg_header);
        virtual void update_fields() = 0;
        virtual void validate_fields() const;

        virtual string raw() = 0;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::HttpMsg<T>::HttpMsg()
{
    add_fields(default_fields());
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::HttpMsg<T>::HttpMsg(const HttpMsg &t_msg)
{
    m_body = t_msg.m_body;
    m_fields = t_msg.m_fields;

    buffer = t_msg.buffer;
    content_type = t_msg.content_type;
    httpv = t_msg.httpv;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::HttpMsg<T>::HttpMsg(const string &t_body,
                                 const string &t_mime) : HttpMsg() {
    if (!t_body.empty())
    {
        body(t_body, t_mime);
    }
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::HttpMsg<T>::HttpMsg(const field_map &t_fields) : HttpMsg()
{
    add_fields(t_fields);
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::HttpMsg<T>::HttpMsg(const field_map &t_fields,
                                 const string &t_body,
                                 const string &t_mime) : HttpMsg(t_body, t_mime) {
    add_fields(t_fields);
}

/// ***
/// Get the HTTP MIME type with the 'charset' parameter set
/// ***
template<class T>
inline std::string scan::HttpMsg<T>::mime_type(const string &t_type,
                                               const string &t_subtype) {

    return Util::fstr("%/%; charset=%", t_type, t_subtype, CHARSET);
}


/// ***
/// Add a new HTTP header field to the underlying field map
/// ***
template<class T>
inline void scan::HttpMsg<T>::add_field(const field_kv &t_field_kvp)
{
    m_fields[normalize_field(t_field_kvp.first)] = t_field_kvp.second;
}

/// ***
/// Add a new HTTP header field to the underlying field map
/// ***
template<class T>
inline void scan::HttpMsg<T>::add_field(const string &t_key, const string &t_val)
{
    add_field(field_kv{ t_key, t_val });
}

/// ***
/// Add the given HTTP header fields to the underlying field map
/// ***
template<class T>
inline void scan::HttpMsg<T>::add_fields(const field_map &t_fields)
{
    for (const field_kv &field_kvp : t_fields)
    {
        add_field(field_kvp);
    }
}

/// ***
/// Get the underlying HTTP message body as a string
/// ***
template<class T>
inline std::string scan::HttpMsg<T>::body() const noexcept
{
    return m_body;
}

/// ***
/// Set the string body value in the underlying HTTP message
/// ***
template<class T>
inline std::string scan::HttpMsg<T>::body(const string &t_body, const string &t_mime)
{
    content_type = t_mime.empty() ? mime_type("text", "plain") : t_mime;
    m_body = t_body;

    add_field("Content-Type", content_type);

    return m_body;
}

/// ***
/// Get the underlying HTTP message header fields as a string
/// ***
template<class T>
inline std::string scan::HttpMsg<T>::raw_fields() const
{
    size_t count{ 0 };
    std::stringstream ss;

    for (const field_kv &kv_pair : m_fields)
    {
        ss << Util::fstr("%: %", kv_pair.first, kv_pair.second);

        if (count++ != m_fields.size() - 1)
        {
            ss << stdu::CRLF;
        }
    }
    return ss.str();
}

/// ***
/// Get the default HTTP message header field map
/// ***
template<class T>
inline typename field_map scan::HttpMsg<T>::default_fields() const
{
    return field_map
    {
        { "Accept",     Util::fstr("%/%", WILDCARD, WILDCARD) },
        { "Connection", CONNECTION }
    };
}

/// ***
/// Retrieve a copy of the underlying message header field map
/// ***
template<class T>
inline typename field_map scan::HttpMsg<T>::msg_fields() const noexcept
{
    return m_fields;
}

/// ***
/// Normalize the casing of the given header key to avoid duplicates
/// ***
template<class T>
inline std::string scan::HttpMsg<T>::normalize_field(const string &t_key)
{
    string header_key;

    // Skip normalization of blank fields
    if (!t_key.empty())
    {
        vector<string> new_parts;

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
        header_key = List<string>::join(new_parts, "-");
    }
    return header_key;
}

/// ***
/// Create a header field map using the given raw HTTP fields
/// ***
template<class T>
inline typename field_map scan::HttpMsg<T>::map(const string &t_raw_fields)
{
    field_map fields;

    // Add headers to underlying dictionary
    if (!t_raw_fields.empty())
    {
        for (const string &header_str : Util::split(t_raw_fields, stdu::CRLF))
        {
            if (header_str.find(":") != string::npos)
            {
                const vector<string> kv_pair{ Util::split(header_str, ":", 1) };
                const string name{ normalize_field(Util::rstrip(kv_pair[0])) };

                fields[name] = Util::lstrip(kv_pair[1]);
            }
        }
    }
    return fields;
}

/// ***
/// Create a header field map using the given HTTP message header
/// ***
template<class T>
inline typename field_map scan::HttpMsg<T>::map(const fields &t_msg_header)
{
    field_map fields;

    for (const field_t &field : t_msg_header)
    {
        const string name{ field.name_string() };
        const string value{ field.value() };

        fields[normalize_field(name)] = value;
    }
    return fields;
}

/// ***
/// Parse the header fields from the given raw message header fields
/// ***
template<class T>
inline void scan::HttpMsg<T>::add_fields(const string &t_raw_fields)
{
    add_fields(map(t_raw_fields));
}

/// ***
/// Parse the header fields from the given HTTP message header
/// ***
template<class T>
inline void scan::HttpMsg<T>::add_fields(const fields &t_msg_header)
{
    add_fields(map(t_msg_header));
}

/// ***
/// Determine whether the given message header contains the specified field
/// ***
template<class T>
inline bool scan::HttpMsg<T>::contains_field(const string &t_field) const
{
    bool found{ false };

    // Search for the field
    for (const field_kv &kv_pair : m_fields)
    {
        if (kv_pair.first == normalize_field(t_field))
        {
            found = true;
            break;
        }
    }
    return found;
}

/// ***
/// Get the 'Content-Length' header field value for the underlying fields
/// ***
template<class T>
inline size_t scan::HttpMsg<T>::content_length() const
{
    size_t length{ 0 };

    if (contains_field("Content-Length"))
    {
        length = std::stoi(m_fields.at("Content-Length"));
    }
    return length;
}

/// ***
/// Validate the HTTP header entries in the given header map
/// ***
template<class T>
inline void scan::HttpMsg<T>::validate_fields() const
{
    if (m_fields.empty())
    {
        throw ArgEx{ "t_fields", "The header map cannot be empty" };
    }
}

#endif // !HTTP_MSG_H
