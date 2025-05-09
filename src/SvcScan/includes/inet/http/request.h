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
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/verb.hpp>
#include "../../concepts/http_concepts.h"
#include "../../errors/error_const_defs.h"
#include "../../errors/runtime_ex.h"
#include "../../ranges/algo.h"
#include "../../ranges/list.h"
#include "../../utils/aliases.h"
#include "../net_aliases.h"
#include "../net_const_defs.h"
#include "message.h"

namespace scan
{
    /**
    * @brief
    *     HTTP network request message.
    */
    template<HttpBody T = string_body_t>
    class Request final : public Message<http::request<T>>
    {
    private:  /* Type Aliases */
        using message_t = typename Message<http::request<T>>::message_t;

    private:  /* Fields */
        verb_t m_method;  // HTTP request method

        string m_host;    // 'Host' header field
        string m_uri;     // HTTP request URI

    public:  /* Constructors & Destructor */
        Request();
        Request(const Request&) = default;
        Request(Request&&) = default;
        Request(const string& t_host, const string& t_uri = URI_ROOT);

        Request(verb_t t_method,
                const string& t_host,
                const string& t_uri = URI_ROOT,
                const string& t_body = {});

        virtual ~Request() = default;

    public:  /* Operators */
        Request& operator=(const Request&) = default;
        Request& operator=(Request&&) = default;

        /**
        * @brief
        *     Bitwise left shift operator overload.
        */
        inline friend ostream& operator<<(ostream& t_os, const Request& t_request)
        {
            return t_os << t_request.raw();
        }

    public:  /* Methods */
        /**
        * @brief
        *     Get the value of the underlying HTTP request method.
        */
        constexpr verb_t method() const noexcept
        {
            return m_method;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying 'Host' HTTP header field.
        */
        constexpr const string& host() const noexcept
        {
            return m_host;
        }

        /**
        * @brief
        *     Get a constant reference to the underlying HTTP request URI.
        */
        constexpr const string& uri() const noexcept
        {
            return m_uri;
        }

        static bool valid_uri(const string& t_uri);

        void method(verb_t t_method);
        void parse(const message_t& t_msg);
        void update_msg() override;
        void uri(const string& t_uri);

        bool valid() const override;

        string method_str() const;
        string start_line() const override;

    private:  /* Methods */
        void host(const string& t_host);
        void validate_headers() const override;
    };
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request() : Message<http::request<T>>{}
{
    m_method = verb_t::head;
    m_uri = URI_ROOT;
    this->m_msg = message_t{m_method, m_uri, this->httpv};

    const List<string> accept_types
    {
        this->mime_type(MIME_TYPE_TEXT),
        this->mime_type(MIME_TYPE_APPLICATION, MIME_SUBTYPE_JSON),
        this->mime_type(MIME_TYPE_APPLICATION, MIME_SUBTYPE_XML)
    };

    this->add_headers(
    {
        {HTTP_ACCEPT,     accept_types.join(",")},
        {HTTP_CONNECTION, CLOSE},
        {HTTP_USER_AGENT, USER_AGENT}
    });
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const string& t_host, const string& t_uri)
    : Request{verb_t::head, t_host, t_uri}
{
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(verb_t t_method,
                                 const string& t_host,
                                 const string& t_uri,
                                 const string& t_body)
    : Request{}
{
    m_host = t_host;
    m_method = t_method;

    this->m_msg = message_t{t_method, t_uri, this->httpv};
    this->add_header(HTTP_HOST, t_host);
    this->body(t_body);

    uri(t_uri);
    update_msg();
}

/**
* @brief
*     Determine whether the given URI is a valid HTTP URI.
*/
template<scan::HttpBody T>
inline bool scan::Request<T>::valid_uri(const string& t_uri)
{
    return algo::matches(t_uri, URI_RGX);
}

/**
* @brief
*     Set the underlying HTTP request method value.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::method(verb_t t_method)
{
    if (t_method != verb_t::unknown)
    {
        this->m_msg.method(m_method = t_method);
    }
}

/**
* @brief
*     Parse information from the given HTTP request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::parse(const message_t& t_msg)
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
*     Set the underlying HTTP request URI value.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::uri(const string& t_uri)
{
    string uri{t_uri};

    if (uri.empty() || !valid_uri(t_uri))
    {
        uri = URI_ROOT;
    }
    this->m_msg.target(m_uri = uri);
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
*     Set the value of the underlying 'Host' HTTP header field.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::host(const string& t_host)
{
    if (!t_host.empty())
    {
        this->add_header(HTTP_HOST, t_host);
    }
}

/**
* @brief
*     Validate the HTTP header entries in the underlying header
*     field map. Throws a runtime exception when validation fails.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::validate_headers() const
{
    const string caller{"Request<T>::validate_headers"};

    if (this->m_headers.empty())
    {
        throw RuntimeEx{EMPTY_HEADER_MAP_MSG, caller};
    }
    header_map_t::const_iterator host_iter{this->m_headers.find(HTTP_HOST)};

    // Missing 'Host' header key
    if (host_iter == this->m_headers.end())
    {
        throw RuntimeEx{algo::fstr(MISSING_HEADER_FMT_MSG, HTTP_HOST), caller};
    }

    // Missing 'Host' header value
    if (host_iter->second.empty())
    {
        throw RuntimeEx{algo::fstr(EMPTY_HEADER_FMT_MSG, HTTP_HOST), caller};
    }
}

#endif // !SCAN_REQUEST_H
