/*
* @file
*     http_msg.h
* @brief
*     Header file for an abstract HTTP message.
*/
#pragma once

#ifndef HTTP_MSG_H
#define HTTP_MSG_H

#include <sdkddkver.h>
#include <boost/beast/core/flat_buffer.hpp>
#include "../../concepts/http_concepts.h"
#include "../../containers/generic/list.h"
#include "../../io/std_util.h"
#include "../../utils/type_defs.h"
#include "../net_expr.h"
#include "http_version.h"

namespace scan
{
    /**
    * @brief  Abstract HTTP network message.
    */
    class HttpMsg : public IStringCastable
    {
    protected:  /* Type Aliases */
        using algo     = Algorithm;
        using buffer_t = beast::flat_buffer;
        using field_t  = http::fields::value_type;
        using fields   = http::fields;
        using stdu     = StdUtil;

    private:  /* Type Aliases */
        using this_t = HttpMsg;

    public:  /* Fields */
        HttpVersion httpv;    // HTTP version

        string content_type;  // 'Content-Type' header
        buffer_t buffer;      // Message buffer

    protected:  /* Fields */
        bool m_chunked;        // Chunked transfer encoding

        string m_body;         // Message body
        header_map m_headers;  // HTTP header fields

    public:  /* Constructors & Destructor */
        HttpMsg();
        HttpMsg(const HttpMsg &t_msg);
        HttpMsg(HttpMsg &&) = default;

        virtual ~HttpMsg() = default;

    public:  /* Operators */
        HttpMsg &operator=(const HttpMsg &) = default;
        HttpMsg &operator=(HttpMsg &&) = default;

    public:  /* Methods */
        static string mime_type(const string &t_type, const string &t_subtype = WILDCARD);

        virtual void add_header(const header_t &t_header);
        virtual void add_header(const string &t_name, const string &t_value);
        void add_headers(const header_map &t_headers);
        virtual void parse(const string &t_raw_msg) = 0;
        virtual void update_msg() = 0;

        bool contains_header(const string &t_name) const;
        bool is_chunked() const noexcept;
        virtual bool valid() const = 0;

        size_t content_length() const;

        virtual const string &body() const noexcept = 0;
        virtual string &body(const string &t_body, const string &t_mime) = 0;
        virtual string msg_header() = 0;
        virtual string raw() const = 0;
        virtual string raw() = 0;
        string raw_headers(const string &t_indent = { }) const;
        virtual string start_line() const = 0;
        virtual string str() const = 0;
        virtual string str() = 0;

        header_map msg_headers() const noexcept;

    protected:  /* Methods */
        static string normalize_header(const string &t_name);

        static header_map map(const string &t_raw_headers);
        static header_map map(const fields &t_fields);

        void add_headers(const string &t_raw_headers);
        void add_headers(const fields &t_fields);
        virtual void update_headers() = 0;
        virtual void validate_headers() const = 0;
    };
}

#endif // !HTTP_MSG_H
