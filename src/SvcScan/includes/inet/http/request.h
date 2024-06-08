/*
* @file
*     request.h
* @brief
*     Header file for an HTTP network request message.
*/
#pragma once

#ifndef SCAN_REQUEST_H
#define SCAN_REQUEST_H

#include <map>
#include <string>
#include <sdkddkver.h>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/verb.hpp>
#include "../../concepts/http_concepts.h"
#include "../../containers/generic/list.h"
#include "../../errors/runtime_ex.h"
#include "../../utils/type_defs.h"
#include "../net_defs.h"
#include "../net_expr.h"
#include "message.h"

namespace scan
{
    /**
    * @brief
    *     HTTP network request message.
    */
    template<HttpBody T = string_body>
    class Request final : public Message<http::request<T>>
    {
    private:  /* Type Aliases */
        using base_t = Message<http::request<T>>;
        using this_t = Request;

        using algo      = typename base_t::algo;
        using message_t = typename base_t::message_t;

    private:  /* Fields */
        verb_t m_method;  // HTTP request method

        string m_host;    // 'Host' header field
        string m_uri;     // HTTP request URI

    public:  /* Constructors & Destructor */
        Request();
        Request(const Request &t_request) noexcept;
        Request(Request &&) = default;
        Request(const string &t_host, const string &t_uri = URI_ROOT);

        Request(const verb_t &t_method,
                const string &t_host,
                const string &t_uri = URI_ROOT,
                const string &t_body = {});

        virtual ~Request() = default;

    public:  /* Operators */
        Request &operator=(const Request &t_request) noexcept;
        Request &operator=(Request &&) = default;

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

        void parse(const message_t &t_msg);
        void update_msg() override;

        bool valid() const override;

        const verb_t &method() const noexcept;
        const verb_t &method(const verb_t &t_method);

        string host() const noexcept;
        string host(const string &t_host);
        string method_str() const;
        string start_line() const override;
        const string &uri() const noexcept;
        string &uri(const string &t_uri);

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
    this->m_msg = message_t{ m_method, m_uri, this->httpv };

    const List<string> accept_types
    {
        this->mime_type(MIME_TYPE_TEXT),
        this->mime_type(MIME_TYPE_APPLICATION, MIME_SUBTYPE_JSON),
        this->mime_type(MIME_TYPE_APPLICATION, MIME_SUBTYPE_XML)
    };

    this->add_headers({
        { HTTP_ACCEPT,     accept_types.join(",") },
        { HTTP_CONNECTION, CLOSE },
        { HTTP_USER_AGENT, USER_AGENT }
    });
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const Request &t_request) noexcept
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

    this->m_msg = message_t{ t_method, t_uri, this->httpv };
    this->add_header(HTTP_HOST, t_host);
    this->body(t_body);

    uri(t_uri);
    update_msg();
}

/**
* @brief
*     Copy assignment operator overload.
*/
template<scan::HttpBody T>
inline scan::Request<T> &scan::Request<T>::operator=(const Request &t_request) noexcept
{
    m_host = t_request.m_host;
    m_method = t_request.m_method;
    m_uri = t_request.m_uri;

    base_t::operator=(t_request);

    return *this;
}

/**
* @brief
*     Determine whether the given URI is a valid HTTP URI.
*/
template<scan::HttpBody T>
inline bool scan::Request<T>::valid_uri(const string &t_uri)
{
    return algo::matches(t_uri, URI_RGX);
}

/**
* @brief
*     Parse information from the given HTTP request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::parse(const message_t &t_msg)
{
    m_uri = static_cast<string>(t_msg.target());
    m_method = t_msg.method();

    this->m_body = t_msg.body();
    this->m_msg = t_msg;

    update_msg();
}

/**
* @brief
*     Update the underlying HTTP request message using the current member values.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::update_msg()
{
    this->update_content_type();
    this->update_member_headers();

    method(m_method);
    uri(m_uri);
    host(m_host);

    this->m_msg.body() = this->m_body;
    this->m_msg.prepare_payload();

    this->update_message_headers();
}

/**
* @brief
*     Determine whether the underlying HTTP request message is valid.
*/
template<scan::HttpBody T>
inline bool scan::Request<T>::valid() const
{
    return method() != verb_t::unknown && this->contains(HTTP_HOST) && valid_uri(m_uri);
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
        this->m_msg.method(m_method = t_method);
    }
    return m_method;
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
        this->add_header(HTTP_HOST, host = t_host);
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
    return static_cast<string>(this->m_msg.method_string());
}

/**
* @brief
*     Get the start-line of the underlying HTTP request header.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::start_line() const
{
    return algo::fstr("% % %", method_str(), m_uri, this->httpv);
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
    this->m_msg.target(m_uri = uri);

    return m_uri;
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

    if (this->m_headers.empty())
    {
        throw RuntimeEx{ caller, "Underlying header map cannot be empty" };
    }
    header_map::const_iterator host_it{ this->m_headers.find(HTTP_HOST) };

    // Missing 'Host' header key
    if (host_it == this->m_headers.end())
    {
        throw RuntimeEx{ caller, algo::fstr("Missing required header '%'", HTTP_HOST) };
    }

    // Missing 'Host' header value
    if (host_it->second.empty())
    {
        throw RuntimeEx{ caller, algo::fstr("Empty '%' header value", HTTP_HOST) };
    }
}

#endif // !SCAN_REQUEST_H
