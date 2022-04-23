/*
*  request.h
*  ---------
*  Header file for a HTTP network request message
*/
#pragma once

#ifndef REQUEST_H
#define REQUEST_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <regex>
#include <sdkddkver.h>
#include <boost/beast/http/parser.hpp>
#include "../../except/runtimeex.h"
#include "../netutil.h"
#include "httpmsg.h"

namespace
{
    namespace asio = boost::asio;
    namespace http = boost::beast::http;
}

namespace scan
{
    /// ***
    /// HTTP network request message
    /// ***
    template<class T = http::string_body>
    class Request final : public HttpMsg<T>
    {
    private:  /* Type Aliases */
        using base = HttpMsg<T>;

        using error_code = boost::system::error_code;
        using field_kv   = std::map<std::string, std::string>::value_type;
        using field_map  = std::map<std::string, std::string>;
        using request_t  = http::request<T>;
        using string     = std::string;
        using verb       = boost::beast::http::verb;

    public:  /* Constants */
        static constexpr char URI_ROOT[] = "/";  // Default URI

    private:  /* Fields */
        verb m_method;    // HTTP request method

        string m_host;    // 'Host' header field
        string m_uri;     // HTTP request URI

        request_t m_req;  // HTTP request message

    public:  /* Constructors & Destructor */
        Request();
        Request(const Request &t_request);

        Request(const verb &t_method,
                const string &t_host,
                const string &t_uri = URI_ROOT,
                const string &t_body = string(),
                const HttpVersion &t_httpv = HttpVersion());

        virtual ~Request() = default;

    public:  /* Operators */
        operator string() const override;

        /// ***
        /// Bitwise left shift operator overload
        /// ***
        inline friend std::ostream &operator<<(std::ostream &t_os,
                                               const Request &t_request) {

            return (t_os << t_request.request());
        }

    public:  /* Methods */
        static bool valid_uri(const string &t_uri);

        void add_field(const field_kv &t_field_kvp) override;
        void add_field(const string &t_key, const string &t_val) override;
        void parse(const request_t &t_req);
        void parse(const string &t_raw_req) override;
        void update_fields() override;
        void update_msg() override;

        bool valid() const override;

        const verb &method() const noexcept;
        verb &method(const verb &t_method);

        string body(const string &t_body,
                    const string &t_mime = string()) override;

        string host() const noexcept;
        string host(const string &t_host);
        string method_str() const;
        string msg_header() override;
        string start_line() const override;
        const string &uri() const noexcept;
        string &uri(const string &t_uri);

        const request_t &request() const noexcept;
        request_t &request() noexcept;

    private:  /* Methods */
        void validate_fields() const override;

        string raw() override;
    };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Request<T>::Request() : base()
{
    m_method = verb::get;
    m_uri = URI_ROOT;
    this->httpv = { 1U, 1U };

    m_req = request_t{ m_method, m_uri, this->httpv };
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Request<T>::Request(const Request &t_request)
{
    m_host = t_request.m_host;
    m_method = t_request.m_method;
    m_req = t_request.m_req;
    m_uri = t_request.m_uri;
    this->m_body = t_request.m_body;
    this->m_fields = t_request.m_fields;

    this->buffer = t_request.buffer;
    this->content_type = t_request.content_type;
    this->httpv = t_request.httpv;
}

/// ***
/// Initialize the object
/// ***
template<class T>
inline scan::Request<T>::Request(const verb &t_method,
                                 const string &t_host,
                                 const string &t_uri,
                                 const string &t_body,
                                 const HttpVersion &t_httpv) : base(t_body) {
    m_host = t_host;
    m_method = t_method;
    m_req = request_t{ t_method, t_uri, t_httpv };
    this->httpv = t_httpv;

    add_field("Host", t_host);
    uri(t_uri);
    update_msg();
}

/// ***
/// Cast operator overload
/// ***
template<class T>
inline scan::Request<T>::operator string() const
{
    return Request(*this).raw();
}

/// ***
/// Determine whether the given URI is a valid HTTP URI
/// ***
template<class T>
inline bool scan::Request<T>::valid_uri(const string &t_uri)
{
    const std::regex pattern{ R"(^([!#$&-;=?-\[\]_a-z~]|%[0-9a-fA-F]{2})+$)" };
    return std::regex_match(t_uri, pattern);
}

/// ***
/// Add a new HTTP header field to the underlying field map and request
/// ***
template<class T>
inline void scan::Request<T>::add_field(const field_kv &t_field_kvp)
{
    this->m_fields[base::normalize_field(t_field_kvp.first)] = t_field_kvp.second;
    m_req.set(base::normalize_field(t_field_kvp.first), t_field_kvp.second);
}

/// ***
/// Add a new HTTP header field to the underlying field map and request
/// ***
template<class T>
inline void scan::Request<T>::add_field(const string &t_key, const string &t_val)
{
    this->m_fields[base::normalize_field(t_key)] = t_val;
    m_req.set(base::normalize_field(t_key), t_val);
}

/// ***
/// Parse information from the given HTTP request
/// ***
template<class T>
inline void scan::Request<T>::parse(const request_t &t_req)
{
    this->m_body = t_req.body();
    m_uri = static_cast<string>(t_req.target());
    m_method = t_req.method();
    m_req = t_req;

    update_msg();
}

/// ***
/// Parse information from the given raw HTTP request
/// ***
template<class T>
inline void scan::Request<T>::parse(const string &t_raw_req)
{
    if (t_raw_req.empty())
    {
        throw ArgEx{ "t_raw_req", "Raw request cannot be empty" };
    }

    size_t offset{ 0 };
    http::request_parser<T> parser;

    // Add buffer data until fully processed
    while (!parser.is_done())
    {
        error_code ecode;

        const string req_data{ t_raw_req.substr(offset) };
        asio::const_buffer req_buffer{ asio::buffer(req_data) };

        // Offset the data start position
        offset += parser.put(req_buffer, ecode);

        if (!NetUtil::no_error(ecode))
        {
            throw RuntimeEx{ "Request<T>::parse", "Failed to parse raw request" };
        }
    }
    parse(parser.get());
}

/// ***
/// Synchronize the underlying request header fields and member header fields
/// ***
template<class T>
inline void scan::Request<T>::update_fields()
{
    // Add 'Content-Type' header
    if (this->m_body.size() > 0)
    {
        if (this->content_type.empty())
        {
            this->content_type = base::mime_type("text", "plain");
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

/// ***
/// Update the underlying HTTP request using the current member values
/// ***
template<class T>
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

/// ***
/// Determine whether the underlying HTTP request message is valid
/// ***
template<class T>
inline bool scan::Request<T>::valid() const
{
    const bool fields_valid{ this->contains_field("Host") };
    const bool method_valid{ method() != verb::unknown };
    const bool uri_valid{ valid_uri(m_uri) };

    return fields_valid && method_valid && uri_valid;
}

/// ***
/// Get the HTTP request message method
/// ***
template<class T>
inline const http::verb &scan::Request<T>::method() const noexcept
{
    return m_method;
}

/// ***
/// Set the HTTP request message method
/// ***
template<class T>
inline http::verb &scan::Request<T>::method(const verb &t_method)
{
    if (t_method != verb::unknown)
    {
        m_req.method(m_method = t_method);
    }
    return m_method;
}

/// ***
/// Set the string body value in the underlying HTTP message
/// ***
template<class T>
inline std::string scan::Request<T>::body(const string &t_body,
                                          const string &t_mime) {
    this->m_body = t_body;
    this->content_type = t_mime.empty() ? base::mime_type("text", "plain") : t_mime;

    update_msg();

    return this->m_body;
}

/// ***
/// Get the value of the 'Host' HTTP header field
/// ***
template<class T>
inline std::string scan::Request<T>::host() const noexcept
{
    return m_host;
}

/// ***
/// Set the value of the 'Host' HTTP header field
/// ***
template<class T>
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

/// ***
/// Get the underlying HTTP request method as a string
/// ***
template<class T>
inline std::string scan::Request<T>::method_str() const
{
    return static_cast<string>(m_req.method_string());
}

/// ***
/// Get the underlying HTTP message header string
/// ***
template<class T>
inline std::string scan::Request<T>::msg_header()
{
    std::stringstream ss;
    ss << m_req.base();

    return ss.str();
}

/// ***
/// Get the start-line of the underlying HTTP request header
/// ***
template<class T>
inline std::string scan::Request<T>::start_line() const
{
    return Util::fstr("% % %", method_str(), m_uri, this->httpv);
}

/// ***
/// Get the target URI from the underlying HTTP request
/// ***
template<class T>
inline const std::string &scan::Request<T>::uri() const noexcept
{
    return m_uri;
}

/// ***
/// Set the target URI on the underlying HTTP request
/// ***
template<class T>
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

/// ***
/// Get a reference to the underlying HTTP request message
/// ***
template<class T>
inline const http::request<T> &scan::Request<T>::request() const noexcept
{
    return m_req;
}

/// ***
/// Get a reference to the underlying HTTP request message
/// ***
template<class T>
inline http::request<T> &scan::Request<T>::request() noexcept
{
    return m_req;
}

/// ***
/// Validate the HTTP header entries in the underlying header field map
/// ***
template<class T>
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

/// ***
/// Get the underlying request as a raw string
/// ***
template<class T>
inline std::string scan::Request<T>::raw()
{
    update_msg();

    std::stringstream ss;
    ss << m_req;

    return ss.str();
}

#endif // !REQUEST_H
