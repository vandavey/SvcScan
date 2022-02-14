/*
*  httpmsg.h
*  ---------
*  Header file for a HTTP message abstract base class
*/
#pragma once

#ifndef HTTP_MSG_H
#define HTTP_MSG_H

#include <map>
#include <string>
#include "../../containers/generic/list.h"
#include "../../io/stdutil.h"

namespace scan
{
    /// ***
    /// Abstract HTTP network message class
    /// ***
    class HttpMsg
    {
    public:  /* Type Aliases */
        using header_map = std::map<std::string, std::string>;
        using header     = header_map::value_type;

    protected:  /* Type Aliases */
        using ulong = unsigned long;

        using sstream = std::stringstream;
        using stdu    = StdUtil;
        using string  = std::string;
        using wstring = std::wstring;

        using list_s   = List<string>;
        using vector_s = std::vector<string>;

    protected: /* Constants */
        static constexpr char CHARSET[]      = "UTF-8";     // MIME charset
        static constexpr char CONNECTION[]   = "close";     // 'Connection' header
        static constexpr char HTTP_VERSION[] = "HTTP/1.1";  // Default HTTP version
        static constexpr char WILDCARD[]     = "*";         // HTTP wild card

    public:  /* Fields */
        string content_type;  // 'Content-Type' header
        string version;       // HTTP version

    protected:  /* Fields */
        size_t m_content_len;  // 'Content-Length' header

        string m_payload;      // HTTP text payload data

        header_map m_headers;  // HTTP header map

    public:  /* Constructors & Destructor */
        HttpMsg(const HttpMsg &t_msg);

        HttpMsg(const string &t_payload,
                const string &t_mime = string());

        HttpMsg(const header_map &t_headers,
                const string &t_payload = string(),
                const string &t_mime = string());

        virtual ~HttpMsg() = default;

    protected:  /* Constructors */
        HttpMsg();

    public:  /* Operators */
        HttpMsg &operator=(const HttpMsg &t_msg) noexcept;

        virtual operator string() const = 0;

        friend std::ostream &operator<<(std::ostream &t_os, const HttpMsg &t_msg);

    public:  /* Methods */
        void add_header(const header &t_header);
        void add_header(const string &t_key, const string &t_val);

        bool contains_header(const string &t_key,
                             const bool &t_not_blank = false) const;

        string get_payload() const noexcept;
        virtual string payload(const string &t_payload, const string &t_mime);
        string raw_headers() const;

        header_map add_headers(const header_map &t_headers);
        header_map default_headers() const;
        header_map get_headers() const noexcept;

    protected:  /* Methods */
        static string mime_type(const string &t_type,
                                const string &t_subtype = WILDCARD);

        static string normalize_header(const string &t_key);

        virtual void validate_headers(const header_map &t_headers) const;

        virtual string raw() = 0;

        virtual header_map update_headers();
    };
}

#endif // !HTTP_MSG_H
