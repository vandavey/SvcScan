/*
* @file
*     response.h
* @brief
*     Header file for an HTTP network response message.
*/
#pragma once

#ifndef SCAN_RESPONSE_H
#define SCAN_RESPONSE_H

#include <map>
#include <string>
#include <sdkddkver.h>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>
#include "../../concepts/http_concepts.h"
#include "../../errors/runtime_ex.h"
#include "../../utils/expr.h"
#include "../../utils/type_defs.h"
#include "../net_defs.h"
#include "../net_expr.h"
#include "message.h"

namespace scan
{
    /**
    * @brief
    *     HTTP network response message.
    */
    template<HttpBody T = string_body>
    class Response final : public Message<http::response<T>>
    {
    private:  /* Type Aliases */
        using base_t = Message<http::response<T>>;
        using this_t = Response;

        using algo      = typename base_t::algo;
        using message_t = typename base_t::message_t;

    private:  /* Fields */
        bool m_valid;       // Response is valid
        status_t m_status;  // Response status

    public:  /* Constructors & Destructor */
        Response() noexcept;
        Response(const Response &t_response) noexcept;
        Response(Response &&) = default;
        Response(const message_t &t_msg);

        virtual ~Response() = default;

    public:  /* Operators */
        Response &operator=(const Response &t_response) noexcept;
        Response &operator=(Response &&) = default;

        operator std::string() const override;

        /**
        * @brief
        *     Bitwise left shift operator overload.
        */
        inline friend ostream &operator<<(ostream &t_os, const Response &t_response)
        {
            return t_os << t_response.raw();
        }

    public:  /* Methods */
        void parse(const message_t &t_msg);
        void update_msg() override;

        bool ok() const noexcept;
        bool unknown() const noexcept;
        bool valid() const override;

        status_t status() const noexcept;
        uint_t status_code() const noexcept;

        const string &body() const noexcept;
        string &body(const string &t_body, const string &t_mime = { });
        string body(const string &t_indent) const;
        string reason() const;
        string server() const;
        string start_line() const override;

    private:  /* Methods */
        void validate_headers() const override;
    };
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response() noexcept : base_t()
{
    m_status = status_t::unknown;
    m_valid = false;
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response(const Response &t_response) noexcept
{
    *this = t_response;
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response(const message_t &t_msg) : this_t()
{
    parse(t_msg);
}

/**
* @brief
*     Copy assignment operator overload.
*/
template<scan::HttpBody T>
inline scan::Response<T> &scan::Response<T>::operator=(const Response &t_response)
    noexcept
{
    m_status = t_response.m_status;
    m_valid = t_response.m_valid;

    base_t::operator=(t_response);

    return *this;
}

/**
* @brief
*     Cast operator overload.
*/
template<scan::HttpBody T>
inline scan::Response<T>::operator std::string() const
{
    string resp_str{ this_t(*this).str() };

    if (unknown())
    {
        resp_str = algo::erase(resp_str, "<unknown-status>");
    }
    return resp_str;
}

/**
* @brief
*     Parse information from the given HTTP response.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::parse(const message_t &t_msg)
{
    m_status = t_msg.result();
    m_valid = m_status != status_t::unknown;

    this->m_body = t_msg.body();
    this->m_msg = t_msg;

    update_msg();
}

/**
* @brief
*     Update the underlying HTTP response message using the current member values.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::update_msg()
{
    this->update_content_type();
    this->update_message_headers();

    this->m_msg.body() = this->m_body;
    this->m_msg.prepare_payload();
    this->m_msg.result(m_status);

    this->update_member_headers();
}

/**
* @brief
*     Determine whether the underlying HTTP response status code is 200 (OK).
*/
template<scan::HttpBody T>
inline bool scan::Response<T>::ok() const noexcept
{
    return status() == status_t::ok;
}

/**
* @brief
*     Determine whether the underlying HTTP response status code is unknown.
*/
template<scan::HttpBody T>
inline bool scan::Response<T>::unknown() const noexcept
{
    return status() == status_t::unknown;
}

/**
* @brief
*     Determine whether the underlying HTTP response message is valid.
*/
template<scan::HttpBody T>
inline bool scan::Response<T>::valid() const
{
    return m_valid;
}

/**
* @brief
*     Get the underlying HTTP response status code as an enumeration type.
*/
template<scan::HttpBody T>
inline scan::http::status scan::Response<T>::status() const noexcept
{
    return m_status;
}

/**
* @brief
*     Get the underlying HTTP response status code as an unsigned integer.
*/
template<scan::HttpBody T>
inline unsigned int scan::Response<T>::status_code() const noexcept
{
    return static_cast<uint_t>(m_status);
}

/**
* @brief
*     Get a constant reference to the underlying HTTP message body.
*/
template<scan::HttpBody T>
inline const std::string &scan::Response<T>::body() const noexcept
{
    return this->m_body;
}

/**
* @brief
*     Set the underlying HTTP message body value and 'Content-Type' header field.
*/
template<scan::HttpBody T>
inline std::string &scan::Response<T>::body(const string &t_body, const string &t_mime)
{
    return base_t::body(t_body, t_mime);
}

/**
* @brief
*     Get the underlying HTTP message body with each
*     line indented using the specified indent string.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::body(const string &t_indent) const
{
    sstream stream;
    const string body_buffer{ algo::replace(this->m_body, CRLF, LF) };

    for (const string &line : algo::split(this->m_body, LF))
    {
        stream << algo::concat(t_indent, line, LF);
    }
    return stream.str();
}

/**
* @brief
*     Get the response phrase of the underlying HTTP response.
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
* @brief
*     Get the value of the underlying 'Server' HTTP header field.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::server() const
{
    string server_str;

    if (this->contains(HTTP_SERVER))
    {
        server_str = this->m_headers.at(HTTP_SERVER);
    }
    return server_str;
}

/**
* @brief
*     Get the start-line of the underlying HTTP response header.
*/
template<scan::HttpBody T>
inline std::string scan::Response<T>::start_line() const
{
    return algo::fstr("% % %", this->httpv, status_code(), reason());
}

/**
* @brief
*     Validate the HTTP header entries in the underlying header
*     field map. Throws a runtime exception when validation fails.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::validate_headers() const
{
    const string caller{ "Response<T>::validate_headers" };

    if (this->m_headers.empty())
    {
        throw RuntimeEx{ caller, "Underlying header map cannot be empty" };
    }
    const header_map::const_iterator server_it{ this->m_headers.find(HTTP_SERVER) };

    // Missing 'Server' header key
    if (server_it == this->m_headers.end())
    {
        throw RuntimeEx{ caller, algo::fstr("Missing required header '%'", HTTP_SERVER) };
    }

    // Missing 'Server' header value
    if (server_it->second.empty())
    {
        throw RuntimeEx{ caller, algo::fstr("Empty '%' header value", HTTP_SERVER) };
    }
}

#endif // !SCAN_RESPONSE_H
