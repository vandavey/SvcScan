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
#include "../../utils/algo.h"
#include "../../utils/aliases.h"
#include "../net_aliases.h"
#include "../net_const_defs.h"
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

        using message_t = typename base_t::message_t;

    private:  /* Fields */
        bool m_valid;       // Response is valid
        status_t m_status;  // Response status

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr Response() noexcept : base_t{}
        {
            m_status = status_t::unknown;
            m_valid = false;
        }

        Response(const Response&) = default;
        Response(Response&&) = default;
        Response(const message_t& t_msg);

        virtual ~Response() = default;

    public:  /* Operators */
        Response& operator=(const Response&) = default;
        Response& operator=(Response&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const override
        {
            string resp_str{this->str()};

            if (unknown())
            {
                resp_str = algo::erase(resp_str, "<unknown-status>");
            }
            return resp_str;
        }

        /**
        * @brief
        *     Bitwise left shift operator overload.
        */
        inline friend ostream& operator<<(ostream& t_os, const Response& t_response)
        {
            return t_os << t_response.raw();
        }

    public:  /* Methods */
        /**
        * @brief
        *     Determine whether the underlying HTTP response status code is 200 (OK).
        */
        constexpr bool ok() const noexcept
        {
            return status() == status_t::ok;
        }

        /**
        * @brief
        *     Determine whether the underlying HTTP response status code is unknown.
        */
        constexpr bool unknown() const noexcept
        {
            return status() == status_t::unknown;
        }

        /**
        * @brief
        *     Determine whether the underlying HTTP response message is valid.
        */
        constexpr bool valid() const override
        {
            return m_valid;
        }

        /**
        * @brief
        *     Get the underlying HTTP response status code as an enumeration type.
        */
        constexpr status_t status() const noexcept
        {
            return m_status;
        }

        /**
        * @brief
        *     Get the underlying HTTP response status code as an unsigned integer.
        */
        constexpr uint_t status_code() const noexcept
        {
            return static_cast<uint_t>(m_status);
        }

        /**
        * @brief
        *     Get the underlying HTTP response status reason phrase.
        */
        constexpr string reason() const
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
        *     Get the start-line of the underlying HTTP response header.
        */
        constexpr string start_line() const override
        {
            return algo::fstr("% % %", this->httpv, status_code(), reason());
        }

        void parse(const message_t& t_msg);
        void update_msg() override;

        string server() const;

    private:  /* Methods */
        void validate_headers() const override;
    };
}

/**
* @brief
*     Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Response<T>::Response(const message_t& t_msg) : Response{}
{
    parse(t_msg);
}

/**
* @brief
*     Parse information from the given HTTP response.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::parse(const message_t& t_msg)
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
*     Validate the HTTP header entries in the underlying header
*     field map. Throws a runtime exception when validation fails.
*/
template<scan::HttpBody T>
inline void scan::Response<T>::validate_headers() const
{
    const string caller{"Response<T>::validate_headers"};

    if (this->m_headers.empty())
    {
        throw RuntimeEx{caller, "Underlying header map cannot be empty"};
    }
    const header_map::const_iterator server_iter{this->m_headers.find(HTTP_SERVER)};

    // Missing 'Server' header key
    if (server_iter == this->m_headers.end())
    {
        throw RuntimeEx{caller, algo::fstr("Missing required header '%'", HTTP_SERVER)};
    }

    // Missing 'Server' header value
    if (server_iter->second.empty())
    {
        throw RuntimeEx{caller, algo::fstr("Empty '%' header value", HTTP_SERVER)};
    }
}

#endif // !SCAN_RESPONSE_H
