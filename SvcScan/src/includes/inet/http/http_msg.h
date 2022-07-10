/*
*  http_msg.h
*  ----------
*  Header file for an abstract HTTP message
*/
#pragma once

#ifndef HTTP_MSG_H
#define HTTP_MSG_H

#include <map>
#include <string>
#include <sdkddkver.h>
#include <boost/beast/core/flat_buffer.hpp>
#include "../../concepts/http_concepts.h"
#include "../../containers/generic/list.h"
#include "../../io/std_util.h"
#include "http_version.h"

namespace
{
    namespace http  = boost::beast::http;
    using field_map = std::map<std::string, std::string>;
}

namespace scan
{
    /**
    * @brief  Abstract HTTP network message.
    */
    template<HttpBody T>
    class HttpMsg : public IStringCastable
    {
    private:  /* Type Aliases */
        using error_code  = boost::system::error_code;
        using fields      = http::fields;
        using field_kv    = field_map::value_type;
        using field_t     = fields::value_type;
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
        bool m_chunked;      // Chunked message encoding

        string m_body;       // Message body
        field_map m_fields;  // HTTP header fields

    public:  /* Constructors & Destructor */
        HttpMsg();
        HttpMsg(const HttpMsg &t_msg);
        HttpMsg(HttpMsg &&) = default;
        HttpMsg(const string &t_body, const string &t_mime = { });
        HttpMsg(const field_map &t_fields);

        HttpMsg(const field_map &t_fields,
                const string &t_body = { },
                const string &t_mime = { });

        virtual ~HttpMsg() = default;

    public:  /* Operators */
        HttpMsg &operator=(const HttpMsg &) = default;
        HttpMsg &operator=(HttpMsg &&) = default;

    public:  /* Methods */
        static string mime_type(const string &t_type,
                                const string &t_subtype = WILDCARD);

        virtual void add_field(const field_kv &t_field_kvp);
        virtual void add_field(const string &t_key, const string &t_val);
        void add_fields(const field_map &t_fields);
        virtual void parse(const string &t_raw_msg) = 0;
        virtual void update_msg() = 0;

        bool contains_field(const string &t_field) const;
        bool is_chunked() const noexcept;
        virtual bool valid() const = 0;

        size_t content_length() const;

        virtual string body() const noexcept;
        virtual string body(const string &t_body, const string &t_mime);
        virtual string msg_header() = 0;
        virtual string raw() const = 0;
        virtual string raw() = 0;
        string raw_fields() const;
        virtual string start_line() const = 0;
        virtual string str() const = 0;
        virtual string str() = 0;

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
    };
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::HttpMsg<T>::HttpMsg()
{
    httpv = { };
    add_fields(default_fields());
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::HttpMsg<T>::HttpMsg(const HttpMsg &t_msg)
{
    m_body = t_msg.m_body;
    m_fields = t_msg.m_fields;
    m_chunked = t_msg.m_chunked;

    buffer = t_msg.buffer;
    content_type = t_msg.content_type;
    httpv = t_msg.httpv;
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::HttpMsg<T>::HttpMsg(const string &t_body,
                                 const string &t_mime) : HttpMsg() {
    if (!t_body.empty())
    {
        body(t_body, t_mime);
    }
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::HttpMsg<T>::HttpMsg(const field_map &t_fields) : HttpMsg()
{
    add_fields(t_fields);
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::HttpMsg<T>::HttpMsg(const field_map &t_fields,
                                 const string &t_body,
                                 const string &t_mime) : HttpMsg(t_body, t_mime) {
    add_fields(t_fields);
}

/**
* @brief  Get the HTTP MIME type with the 'charset' parameter set.
*/
template<scan::HttpBody T>
inline std::string scan::HttpMsg<T>::mime_type(const string &t_type,
                                               const string &t_subtype) {

    return Util::fstr("%/%; charset=%", t_type, t_subtype, CHARSET);
}

/**
* @brief  Add a new HTTP header field to the underlying field map.
*/
template<scan::HttpBody T>
inline void scan::HttpMsg<T>::add_field(const field_kv &t_field_kvp)
{
    add_field(t_field_kvp.first, t_field_kvp.second);
}

/**
* @brief  Add a new HTTP header field to the underlying field map.
*/
template<scan::HttpBody T>
inline void scan::HttpMsg<T>::add_field(const string &t_key, const string &t_val)
{
    m_fields[normalize_field(t_key)] = t_val;
}

/**
* @brief  Add the given HTTP header field to the underlying field map.
*/
template<scan::HttpBody T>
inline void scan::HttpMsg<T>::add_fields(const field_map &t_fields)
{
    for (const field_kv &field_kvp : t_fields)
    {
        add_field(field_kvp);
    }
}

/**
* @brief  Get the underlying HTTP message body as a string.
*/
template<scan::HttpBody T>
inline std::string scan::HttpMsg<T>::body() const noexcept
{
    return m_body;
}

/**
* @brief  Set the underlying HTTP message body value.
*/
template<scan::HttpBody T>
inline std::string scan::HttpMsg<T>::body(const string &t_body, const string &t_mime)
{
    content_type = t_mime.empty() ? mime_type("text", "plain") : t_mime;
    m_body = t_body;

    add_field("Content-Type", content_type);

    return m_body;
}

/**
* @brief  Get the underlying HTTP message header fields as a string.
*/
template<scan::HttpBody T>
inline std::string scan::HttpMsg<T>::raw_fields() const
{
    size_t count{ 0 };
    std::stringstream sstream;

    for (const field_kv &kv_pair : m_fields)
    {
        sstream << Util::fstr("%: %", kv_pair.first, kv_pair.second);

        if (count++ != m_fields.size() - 1)
        {
            sstream << stdu::CRLF;
        }
    }
    return sstream.str();
}

/**
* @brief  Get the default HTTP message header field map.
*/
template<scan::HttpBody T>
inline typename field_map scan::HttpMsg<T>::default_fields() const
{
    return field_map
    {
        { "Accept",     Util::fstr("%/%", WILDCARD, WILDCARD) },
        { "Connection", CONNECTION }
    };
}

/**
* @brief  Make a copy of the underlying message header field map.
*/
template<scan::HttpBody T>
inline typename field_map scan::HttpMsg<T>::msg_fields() const noexcept
{
    return m_fields;
}

/**
* @brief  Normalize the casing of the given header field key to avoid duplicates.
*/
template<scan::HttpBody T>
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
        header_key = List<string>(new_parts).join("-");
    }
    return header_key;
}

/**
* @brief  Create a header field map using the given raw HTTP fields.
*/
template<scan::HttpBody T>
inline typename field_map scan::HttpMsg<T>::map(const string &t_raw_fields)
{
    field_map fields;

    // Add headers to underlying map
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

/**
* @brief  Create a header field map using the given HTTP message header.
*/
template<scan::HttpBody T>
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

/**
* @brief  Parse the header fields from the given raw message header fields.
*/
template<scan::HttpBody T>
inline void scan::HttpMsg<T>::add_fields(const string &t_raw_fields)
{
    add_fields(map(t_raw_fields));
}

/**
* @brief  Parse the header fields from the given HTTP message header.
*/
template<scan::HttpBody T>
inline void scan::HttpMsg<T>::add_fields(const fields &t_msg_header)
{
    add_fields(map(t_msg_header));
}

/**
* @brief  Determine whether the underlying header map contains the given field.
*/
template<scan::HttpBody T>
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

/**
* @brief  Determine whether the underlying HTTP message is
*         using chunked transfer encoding.
*/
template<scan::HttpBody T>
inline bool scan::HttpMsg<T>::is_chunked() const noexcept
{
    return m_chunked;
}

/**
* @brief  Get the 'Content-Length' header field value from the underlying field map.
*/
template<scan::HttpBody T>
inline size_t scan::HttpMsg<T>::content_length() const
{
    size_t length{ 0 };

    if (contains_field("Content-Length"))
    {
        length = std::stoi(m_fields.at("Content-Length"));
    }
    return length;
}

/**
* @brief  Validate the HTTP header entries in the underlying header field map.
*/
template<scan::HttpBody T>
inline void scan::HttpMsg<T>::validate_fields() const
{
    if (m_fields.empty())
    {
        throw RuntimeEx("HttpMsg<T>::validate_fields",
                        "The underlying field map cannot be empty");
    }
}

#endif // !HTTP_MSG_H
