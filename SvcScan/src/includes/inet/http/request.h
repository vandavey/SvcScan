/*
*  request.h
*  ---------
*  Header file for an HTTP network request message
*/
#pragma once

#ifndef REQUEST_H
#define REQUEST_H

#include <regex>
#include <sdkddkver.h>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/write.hpp>
#include "../../except/runtime_ex.h"
#include "../net_util.h"
#include "http_msg.h"

namespace scan
{
    namespace
    {
        namespace asio = boost::asio;
        namespace http = boost::beast::http;
    }

    /**
    * @brief  HTTP network request message.
    */
    template<HttpBody T = http::string_body>
    class Request final : public HttpMsg<T>
    {
    private:  /* Type Aliases */
        using base_t = HttpMsg<T>;
        using this_t = Request;

        using algo       = Algorithm;
        using error_code = boost::system::error_code;
        using field_kv   = std::map<std::string, std::string>::value_type;
        using field_map  = std::map<std::string, std::string>;
        using message_t  = http::request<T>;
        using string     = std::string;
        using verb_t     = boost::beast::http::verb;

    public:  /* Constants */
        static constexpr char URI_ROOT[] = "/";  // Default URI

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
                const string &t_body = { },
                const HttpVersion &t_httpv = { });

        virtual ~Request() = default;

    public:  /* Operators */
        Request &operator=(const Request &t_request);
        Request &operator=(Request &&) = default;

        operator string() const override;

        /**
        * @brief  Bitwise left shift operator overload.
        */
        inline friend std::ostream &operator<<(std::ostream &t_os,
                                               const Request &t_request) {
            return t_os << t_request.raw();
        }

    public:  /* Methods */
        static bool valid_uri(const string &t_uri);

        void add_field(const field_kv &t_field_kvp) override;
        void add_field(const string &t_key, const string &t_val) override;
        void parse(const message_t &t_msg);
        void parse(const string &t_raw_msg) override;
        void update_fields() override;
        void update_msg() override;

        bool valid() const override;

        const verb_t &method() const noexcept;
        const verb_t &method(const verb_t &t_method);

        string body(const string &t_body, const string &t_mime = { }) override;
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
        void validate_fields() const override;
    };
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request() : base_t()
{
    m_method = verb_t::get;
    m_req = message_t{ m_method, m_uri, this->httpv };
    m_uri = URI_ROOT;
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const Request &t_request)
{
    *this = t_request;
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const string &t_host, const string &t_uri)
    : this_t(verb_t::get, t_host) {
}

/**
* @brief  Initialize the object.
*/
template<scan::HttpBody T>
inline scan::Request<T>::Request(const verb_t &t_method,
                                 const string &t_host,
                                 const string &t_uri,
                                 const string &t_body,
                                 const HttpVersion &t_httpv) : base_t(t_body) {
    m_host = t_host;
    m_method = t_method;
    m_req = message_t{ t_method, t_uri, t_httpv };
    this->httpv = t_httpv;

    add_field("Host", t_host);
    uri(t_uri);
    update_msg();
}

/**
* @brief  Copy assignment operator overload.
*/
template<scan::HttpBody T>
inline scan::Request<T> &scan::Request<T>::operator=(const Request &t_request)
{
    m_host = t_request.m_host;
    m_method = t_request.m_method;
    m_req = t_request.m_req;
    m_uri = t_request.m_uri;
    this->m_body = t_request.m_body;
    this->m_chunked = t_request.m_chunked;
    this->m_fields = t_request.m_fields;

    this->buffer = t_request.buffer;
    this->content_type = t_request.content_type;
    this->httpv = t_request.httpv;

    return *this;
}

/**
* @brief  Cast operator overload.
*/
template<scan::HttpBody T>
inline scan::Request<T>::operator string() const
{
    return this_t(*this).str();
}

/**
* @brief  Determine whether the given URI is a valid HTTP URI.
*/
template<scan::HttpBody T>
inline bool scan::Request<T>::valid_uri(const string &t_uri)
{
    const std::regex pattern{ R"(^([!#$&-;=?-\[\]_a-z~]|%[0-9a-fA-F]{2})+$)" };
    return std::regex_match(t_uri, pattern);
}

/**
* @brief  Add a new HTTP header field to the underlying field map and request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::add_field(const field_kv &t_field_kvp)
{
    add_field(t_field_kvp.first, t_field_kvp.second);
}

/**
* @brief  Add a new HTTP header field to the underlying field map and request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::add_field(const string &t_key, const string &t_val)
{
    this->m_fields[base_t::normalize_field(t_key)] = t_val;
    m_req.set(base_t::normalize_field(t_key), t_val);
}

/**
* @brief  Parse information from the given HTTP request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::parse(const message_t &t_msg)
{
    this->m_body = t_msg.body();
    m_uri = static_cast<string>(t_msg.target());
    m_method = t_msg.method();
    m_req = t_msg;

    update_msg();
}

/**
* @brief  Parse information from the given raw HTTP request.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::parse(const string &t_raw_msg)
{
    if (t_raw_msg.empty())
    {
        throw ArgEx{ "t_raw_msg", "Raw request cannot be empty" };
    }
    string raw_msg{ t_raw_msg };

    if (!raw_msg.ends_with(StdUtil::CRLF))
    {
        raw_msg += StdUtil::CRLF;
    }
    size_t offset{ 0 };

    error_code ecode;
    http::request_parser<T> parser;

    do  // Parse the entire raw HTTP request
    {
        const string req_data{ raw_msg.substr(offset) };
        const size_t bytes_read{ parser.put(asio::buffer(req_data), ecode) };

        // Inform parser that EOF was reached
        if (bytes_read <= 0)
        {
            error_code put_eof_ecode;
            parser.put_eof(put_eof_ecode);
        }
        offset += bytes_read;
    }
    while (!parser.is_done());

    parse(parser.get());
}

/**
* @brief  Synchronize the underlying request header fields and member header fields.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::update_fields()
{
    // Add 'Content-Type' header
    if (this->m_body.size() > 0)
    {
        if (this->content_type.empty())
        {
            this->content_type = base_t::mime_type("text", "plain");
        }
        add_field("Content-Type", this->content_type);
    }

    // Update request using member headers
    for (const field_kv &field_kvp : this->m_fields)
    {
        m_req.set(field_kvp.first, field_kvp.second);
    }
    this->add_fields(m_req.base());
}

/**
* @brief  Update the underlying HTTP request using the current member values.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::update_msg()
{
    update_fields();

    method(m_method);
    uri(m_uri);
    host(m_host);

    m_req.body() = this->m_body;
    m_req.prepare_payload();

    update_fields();
}

/**
* @brief  Determine whether the underlying HTTP request message is valid.
*/
template<scan::HttpBody T>
inline bool scan::Request<T>::valid() const
{
    const bool fields_valid{ this->contains_field("Host") };
    const bool method_valid{ method() != verb_t::unknown };
    const bool uri_valid{ valid_uri(m_uri) };

    return fields_valid && method_valid && uri_valid;
}

/**
* @brief  Get a constant reference to the underlying HTTP request method.
*/
template<scan::HttpBody T>
inline const scan::http::verb &scan::Request<T>::method() const noexcept
{
    return m_method;
}

/**
* @brief  Set the underlying HTTP request method value.
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
* @brief  Set the underlying HTTP request body value.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::body(const string &t_body, const string &t_mime)
{
    this->m_body = t_body;
    this->content_type = t_mime;

    if (this->content_type.empty())
    {
        this->content_type = base_t::mime_type("text", "plain");
    }
    update_msg();

    return this->m_body;
}

/**
* @brief  Get the value of the underlying 'Host' HTTP header field.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::host() const noexcept
{
    return m_host;
}

/**
* @brief  Set the value of the underlying 'Host' HTTP header field.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::host(const string &t_host)
{
    string host;

    if (!t_host.empty())
    {
        add_field("Host", host = t_host);
        update_fields();    
    }
    return host;
}

/**
* @brief  Get the underlying HTTP request method as a string.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::method_str() const
{
    return static_cast<string>(m_req.method_string());
}

/**
* @brief  Get the underlying HTTP request header as a string.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::msg_header()
{
    return algo::to_string(m_req.base());
}

/**
* @brief  Get the underlying HTTP request as a string. Chunked
*         transfer-encoding chunk sizes will be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::raw() const
{
    return this_t(*this).raw();
}

/**
* @brief  Get the underlying HTTP request as a string. Chunked
*         transfer-encoding chunk sizes will be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::raw()
{
    update_msg();
    return algo::to_string(m_req);
}

/**
* @brief  Get the start-line of the underlying HTTP request header.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::start_line() const
{
    return algo::fstr("% % %", method_str(), m_uri, this->httpv);
}

/**
* @brief  Get the underlying HTTP request as a string. Chunked
*         transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::str() const
{
    return this_t(*this).str();
}

/**
* @brief  Get the underlying HTTP request as a string. Chunked
*         transfer-encoding chunk sizes will not be included.
*/
template<scan::HttpBody T>
inline std::string scan::Request<T>::str()
{
    std::stringstream sstream;

    update_msg();
    sstream << m_req.base();
    sstream << m_req.body();

    return sstream.str();
}

/**
* @brief  Get a constant reference to the underlying HTTP request URI.
*/
template<scan::HttpBody T>
inline const std::string &scan::Request<T>::uri() const noexcept
{
    return m_uri;
}

/**
* @brief  Get a reference to the underlying HTTP request URI.
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
* @brief  Get a constant reference to the underlying HTTP request message.
*/
template<scan::HttpBody T>
inline const scan::http::request<T> &scan::Request<T>::message() const noexcept
{
    return m_req;
}

/**
* @brief  Get a reference to the underlying HTTP request message.
*/
template<scan::HttpBody T>
inline scan::http::request<T> &scan::Request<T>::message() noexcept
{
    return m_req;
}

/**
* @brief  Validate the HTTP header entries in the underlying header field map.
*/
template<scan::HttpBody T>
inline void scan::Request<T>::validate_fields() const
{
    const string caller{ "Request<T>::validate_fields" };
    field_map::const_iterator host_it{ this->m_fields.find("Host") };

    // Missing 'Host' header key
    if (host_it == this->m_fields.end())
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
