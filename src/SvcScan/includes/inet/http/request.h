/*
* @file
*     request.h
* @brief
*     Header file for an HTTP network request message.
*/
#pragma once

#ifndef REQUEST_H
#define REQUEST_H

#include <sdkddkver.h>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/write.hpp>
#include "../../errors/runtime_ex.h"
#include "http_msg.h"

namespace scan
{
    /**
    * @brief
    *     HTTP network request message.
    */
    template<HttpBody T = string_body>
    class Request final : public HttpMsg
    {
    private:  /* Type Aliases */
        using base_t = HttpMsg;
        using this_t = Request;

        using message_t = http::request<T>;

    private:  /* Fields */
        verb_t m_method;  // HTTP request method

        string m_host;    // 'Host' header field
        string m_uri;     // HTTP request URI

        message_t m_req;  // HTTP request message

    public:  /* Constructors & Destructor */
        Request();
        Request(const Request &t_request);
        Request(Request &&) = default;
        Request(const string &t_host, const string &t_uri = URI_ROOT);

        Request(const verb_t &t_method,
                const string &t_host,
                const string &t_uri = URI_ROOT,
                const string &t_body = { });

        virtual ~Request() = default;

    public:  /* Operators */
        Request &operator=(const Request &t_request);
        Request &operator=(Request &&) = default;

        operator std::string() const override;

        /**
        * @brief
        *     Bitwise left shift operator overload.
        */
        inline friend ostream &operator<<(ostream &t_os, const Request &t_request)
        {
            return t_os << t_request.raw();
        }

    public:  /* Methods */
        static bool valid_uri(const string &t_uri);

        void add_header(const header_t &t_header) override;
        void add_header(const string &t_name, const string &t_value) override;
        void parse(const message_t &t_msg);
        void update_member_headers() override;
        void update_message_headers() override;
        void update_msg() override;

        bool valid() const override;

        const verb_t &method() const noexcept;
        const verb_t &method(const verb_t &t_method);

        const string &body() const noexcept override;
        string &body(const string &t_body, const string &t_mime = { }) override;
        string host() const noexcept;
        string host(const string &t_host);
        string method_str() const;
        string msg_header() override;
        string raw() const override;
        string raw() override;
        string start_line() const override;
        string str() const override;
        string str() override;
        const string &uri() const noexcept;
        string &uri(const string &t_uri);

        const message_t &message() const noexcept;
        message_t &message() noexcept;

    private:  /* Methods */
        void validate_headers() const override;
    };
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request() : base_t()
{
    m_method = verb_t::head;
    m_uri = URI_ROOT;
    m_req = message_t{ m_method, m_uri, httpv };

    const List<string> accept_types
    {
        mime_type("text"),
        mime_type("application", "json"),
        mime_type("application", "xml")
    };

    add_headers({
        { "Accept",     accept_types.join(",") },
        { "Connection", CONNECTION },
        { "User-Agent", USER_AGENT }
    });
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const Request &t_request)
{
    *this = t_request;
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const string &t_host, const string &t_uri)
    : this_t(verb_t::head, t_host, t_uri)
{
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const verb_t &t_method,
                                 const string &t_host,
                                 const string &t_uri,
                                 const string &t_body)
    : this_t()
{
    m_host = t_host;
    m_method = t_method;
    m_req = message_t{ t_method, t_uri, httpv };

    add_header("Host", t_host);
    body(t_body);
    uri(t_uri);
    update_msg();
}

/**
* @brief
*     Copy assignment operator overload.
*/
template<scan::HttpBody T>
inline scan::Request<T> &scan::Request<T>::operator=(const Request &t_request)
{
    m_body = t_request.m_body;
    m_chunked = t_request.m_chunked;
    m_content_type = t_request.m_content_type;
    m_headers = t_request.m_headers;
    m_host = t_request.m_host;
    m_method = t_request.m_method;
    m_req = t_request.m_req;
    m_uri = t_request.m_uri;

    buffer = t_request.buffer;
    httpv = t_request.httpv;

    return *this;
}

/**
* @brief
*     Cast operator overload.
*/
template<scan::HttpBody T>
inline scan::Request<T>::operator std::string() const
{
    return this_t(*this).str();
}

/**
* @brief
*     Determine whether the given URI is a valid HTTP URI.
*/
template<scan::HttpBody T>
inline bool scan::Request<T>::valid_uri(const string &t_uri)
{
    return algo::matches(t_uri, R"(^([!#$&-;=?-\[\]_a-z~]|%[0-9a-fA-F]{2})+$)");
}

/**
* @brief
*     Add a new HTTP header field to the underlying header field map and request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::add_header(const header_t &t_header)
{
    add_header(t_header.first, t_header.second);
}

/**
* @brief
*     Add a new HTTP header field to the underlying header field map and request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::add_header(const string &t_name, const string &t_value)
{
    m_headers[normalize_header(t_name)] = t_value;
    m_req.set(normalize_header(t_name), t_value);
}

/**
* @brief
*     Parse information from the given HTTP request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::parse(const message_t &t_msg)
{
    m_body = t_msg.body();
    m_uri = static_cast<string>(t_msg.target());
    m_method = t_msg.method();
    m_req = t_msg;

    update_msg();
}

/**
* @brief
*     Update the underlying HTTP header field map member using
*     the current values of the HTTP request message member.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::update_member_headers()
{
    add_headers(m_req.base());
}

/**
* @brief
*     Update the underlying HTTP request message member using
*     the current values of the HTTP header field map member.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::update_message_headers()
{
    for (const header_t &header : m_headers)
    {
        m_req.set(header.first, header.second);
    }
}

/**
* @brief
*     Update the underlying HTTP request message using the current member values.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::update_msg()
{
    update_content_type();
    update_member_headers();

    method(m_method);
    uri(m_uri);
    host(m_host);

    m_req.body() = m_body;
    m_req.prepare_payload();

    update_message_headers();
}

/**
* @brief
*     Determine whether the underlying HTTP request message is valid.
*/
template<scan::HttpBody T>
inline bool scan::Request<T>::valid() const
{
    return contains_header("Host") && method() != verb_t::unknown && valid_uri(m_uri);
}

/**
* @brief
*     Get a constant reference to the underlying HTTP request method.
*/
template<scan::HttpBody T>
inline const scan::http::verb &scan::Request<T>::method() const noexcept
{
    return m_method;
}

/**
* @brief
*     Set the underlying HTTP request method value.
*/
template<scan::HttpBody T>
inline const scan::http::verb &scan::Request<T>::method(const verb_t &t_method)
{
    if (t_method != verb_t::unknown)
    {
        m_req.method(m_method = t_method);
    }
    return m_method;
}

/**
* @brief
*     Get a constant reference to the underlying HTTP message body.
*/
template<scan::HttpBody T>
inline const std::string &scan::Request<T>::body() const noexcept
{
    return m_body;
}

/**
* @brief
*     Set the underlying HTTP request body value.
*/
template<scan::HttpBody T>
inline std::string &scan::Request<T>::body(const string &t_body, const string &t_mime)
{
    m_body = t_body;
    m_content_type = t_mime;

    if (m_content_type.empty())
    {
        m_content_type = mime_type("text", "plain");
    }
    update_msg();

    return m_body;
}

/**
* @brief
*     Get the value of the underlying 'Host' HTTP header field.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::host() const noexcept
{
    return m_host;
}

/**
* @brief
*     Set the value of the underlying 'Host' HTTP header field.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::host(const string &t_host)
{
    string host;

    if (!t_host.empty())
    {
        add_header("Host", host = t_host);
    }
    return host;
}

/**
* @brief
*     Get the underlying HTTP request method as a string.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::method_str() const
{
    return static_cast<string>(m_req.method_string());
}

/**
* @brief
*     Get the underlying HTTP request header as a string.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::msg_header()
{
    return algo::to_string(m_req.base());
}

/**
* @brief
*     Get the underlying HTTP request as a string. Chunked
*     transfer-encoding chunk sizes will be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::raw() const
{
    return algo::to_string(m_req);
}

/**
* @brief
*     Get the underlying HTTP request as a string. Chunked
*     transfer-encoding chunk sizes will be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::raw()
{
    return algo::to_string(m_req);
}

/**
* @brief
*     Get the start-line of the underlying HTTP request header.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::start_line() const
{
    return algo::fstr("% % %", method_str(), m_uri, httpv);
}

/**
* @brief
*     Get the underlying HTTP request as a string. Chunked
*     transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::str() const
{
    return this_t(*this).str();
}

/**
* @brief
*     Get the underlying HTTP request as a string. Chunked
*     transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::str()
{
    sstream stream;

    update_msg();
    stream << m_req.base() << m_req.body();

    return stream.str();
}

/**
* @brief
*     Get a constant reference to the underlying HTTP request URI.
*/
template<scan::HttpBody T>
inline const std::string &scan::Request<T>::uri() const noexcept
{
    return m_uri;
}

/**
* @brief
*     Get a reference to the underlying HTTP request URI.
*/
template<scan::HttpBody T>
inline std::string &scan::Request<T>::uri(const string &t_uri)
{
    string uri{ t_uri };

    if (uri.empty() || !valid_uri(t_uri))
    {
        uri = URI_ROOT;
    }
    m_req.target(m_uri = uri);

    return m_uri;
}

/**
* @brief
*     Get a constant reference to the underlying HTTP request message.
*/
template<scan::HttpBody T>
inline const scan::http::request<T> &scan::Request<T>::message() const noexcept
{
    return m_req;
}

/**
* @brief
*     Get a reference to the underlying HTTP request message.
*/
template<scan::HttpBody T>
inline scan::http::request<T> &scan::Request<T>::message() noexcept
{
    return m_req;
}

/**
* @brief
*     Validate the HTTP header entries in the underlying header
*     field map. Throws a runtime exception when validation fails.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::validate_headers() const
{
    const string caller{ "Request<T>::validate_headers" };

    if (m_headers.empty())
    {
        throw RuntimeEx{ caller, "Underlying header map cannot be empty" };
    }
    header_map::const_iterator host_it{ m_headers.find("Host") };

    // Missing 'Host' header key
    if (host_it == m_headers.end())
    {
        throw RuntimeEx{ caller, "Missing required header 'Host'" };
    }

    // Missing 'Host' header value
    if (host_it->second.empty())
    {
        throw RuntimeEx{ caller, "Missing value for required header 'Host'" };
    }
}

#endif // !REQUEST_H
