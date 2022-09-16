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

namespace scan
{
    namespace
    {
        namespace http = boost::beast::http;
    }

    /**
    * @brief  Abstract HTTP network message.
    */
    class HttpMsg : public IStringCastable
    {
    protected:  /* Type Aliases */
        using algo        = Algorithm;
        using error_code  = boost::system::error_code;
        using field_t     = http::fields::value_type;
        using fields      = http::fields;
        using flat_buffer = boost::beast::flat_buffer;
        using header_map  = std::map<std::string, std::string>;
        using header_t    = header_map::value_type;
        using stdu        = StdUtil;
        using string      = std::string;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Type Aliases */
        using this_t = HttpMsg;

    protected: /* Constants */
        static constexpr char CHARSET[]    = "UTF-8";  // MIME character set
        static constexpr char CONNECTION[] = "close";  // Default 'Connection' header
        static constexpr char WILDCARD[]   = "*";      // MIME wild card

    public:  /* Fields */
        HttpVersion httpv;    // HTTP version

        string content_type;  // 'Content-Type' header
        flat_buffer buffer;   // Message buffer

    protected:  /* Fields */
        bool m_chunked;        // Chunked transfer encoding

        string m_body;         // Message body
        header_map m_headers;  // HTTP header fields

    public:  /* Constructors & Destructor */
        HttpMsg();
        HttpMsg(const HttpMsg &t_msg);
        HttpMsg(HttpMsg &&) = default;
        HttpMsg(const string &t_body, const string &t_mime = { });
        HttpMsg(const header_map &t_headers);

        HttpMsg(const header_map &t_headers,
                const string &t_body = { },
                const string &t_mime = { });

        virtual ~HttpMsg() = default;

    public:  /* Operators */
        HttpMsg &operator=(const HttpMsg &) = default;
        HttpMsg &operator=(HttpMsg &&) = default;

    public:  /* Methods */
        static string mime_type(const string &t_type,
                                const string &t_subtype = WILDCARD);

        virtual void add_header(const header_t &t_header);
        virtual void add_header(const string &t_name, const string &t_value);
        void add_headers(const header_map &t_headers);
        virtual void parse(const string &t_raw_msg) = 0;
        virtual void update_msg() = 0;

        bool contains_header(const string &t_name) const;
        bool is_chunked() const noexcept;
        virtual bool valid() const = 0;

        size_t content_length() const;

        virtual string body() const noexcept;
        virtual string body(const string &t_body, const string &t_mime);
        virtual string msg_header() = 0;
        virtual string raw() const = 0;
        virtual string raw() = 0;
        string raw_headers() const;
        virtual string start_line() const = 0;
        virtual string str() const = 0;
        virtual string str() = 0;

        virtual header_map default_headers() const;
        header_map msg_headers() const noexcept;

    protected:  /* Methods */
        static string normalize_header(const string &t_name);

        static header_map map(const string &t_raw_headers);
        static header_map map(const fields &t_fields);

        void add_headers(const string &t_raw_headers);
        void add_headers(const fields &t_fields);
        virtual void update_headers() = 0;
        virtual void validate_headers() const;
    };
}

#endif // !HTTP_MSG_H
