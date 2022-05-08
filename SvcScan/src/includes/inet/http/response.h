/*
*  response.h
*  ----------
*  Header file for a HTTP network response message
*/
#pragma once

#ifndef RESPONSE_H
#define RESPONSE_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <boost/beast/http/parser.hpp>
#include "httpmsg.h"

namespace
{
    namespace asio = boost::asio;
    namespace http = boost::beast::http;
}

namespace scan
{
    /// ***
    /// HTTP network response message
    /// ***
    template<HttpBody T = http::string_body>
    class Response final : public HttpMsg<T>
    {
    public:  /* Type Aliases */
        using value_type = T;

    private:  /* Type Aliases */
        using base = HttpMsg<T>;

        using uint = unsigned int;

        using error_code = boost::system::error_code;
        using field_kv   = std::map<std::string, std::string>::value_type;
        using field_map  = std::map<std::string, std::string>;
        using response_t = http::response<T>;
        using status_t   = http::status;
        using string     = std::string;

    private:  /* Fields */
        bool m_valid;       // Response is valid

        status_t m_status;  // Response status code
        response_t m_resp;  // HTTP response message

    public:  /* Constructors & Destructor */
        Response();
        Response(const Response &t_response);
        Response(const response_t &t_resp);
        Response(const string &t_raw_response);

        virtual ~Response() = default;

    public:  /* Operators */
        operator string() const override;

        /// ***
        /// Bitwise left shift operator overload
        /// ***
        inline friend std::ostream &operator<<(std::ostream &t_os,
                                               const Response &t_response) {

            return (t_os << t_response.response());
        }

    public:  /* Methods */
        void add_field(const field_kv &t_field_kvp) override;
        void add_field(const string &t_key, const string &t_val) override;
        void parse(const response_t &t_resp);
        void parse(const string &t_raw_resp) override;
        void update_fields() override;
        void update_msg() override;

        bool valid() const override;
        bool ok() const noexcept;

        status_t status() const noexcept;
        uint status_code() const noexcept;

        string body(const string &t_body, const string &t_mime) override;
        string msg_header() override;
        string server() const;
        string start_line() const override;
        string status_str() const;

        const response_t &response() const noexcept;
        response_t &response() noexcept;

    private:  /* Methods */
        void validate_fields() const override;

        string raw() override;
    };
}

/// ***
/// Initialize the object
/// ***
template<scan::HttpBody T>
inline scan::Response<T>::Response() : base()
{
    m_status = status_t::unknown;
    m_valid = false;
    this->m_fields.clear();
}

/// ***
/// Initialize the object
/// ***
template<scan::HttpBody T>
inline scan::Response<T>::Response(const Response &t_response)
{
    m_resp = t_response.m_resp;
    m_status = t_response.m_status;
    m_valid = t_response.m_valid;
    this->m_body = t_response.m_body;
    this->m_fields = t_response.m_fields;

    this->buffer = t_response.buffer;
    this->content_type = t_response.content_type;
    this->httpv = t_response.httpv;
}

/// ***
/// Initialize the object
/// ***
template<scan::HttpBody T>
inline scan::Response<T>::Response(const response_t &t_resp)
{
    parse(t_resp);
}

/// ***
/// Initialize the object
/// ***
template<scan::HttpBody T>
inline scan::Response<T>::Response(const string &t_raw_response)
{
    parse(t_raw_response);
}

/// ***
/// Cast operator overload
/// ***
template<scan::HttpBody T>
inline scan::Response<T>::operator string() const
{
    return Response(*this).raw();
}

/// ***
/// Add a new HTTP header to the underlying field map and response
/// ***
template<scan::HttpBody T>
inline void scan::Response<T>::add_field(const field_kv &t_field_kvp)
{
    this->m_fields[base::normalize_field(t_field_kvp.first)] = t_field_kvp.second;
    m_resp.set(base::normalize_field(t_field_kvp.first), t_field_kvp.second);
}

/// ***
/// Add a new HTTP header field to the underlying field map and response
/// ***
template<scan::HttpBody T>
inline void scan::Response<T>::add_field(const string &t_key, const string &t_val)
{
    this->m_fields[base::normalize_field(t_key)] = t_val;
    m_resp.set(base::normalize_field(t_key), t_val);
}

/// ***
/// Parse information from the given HTTP response
/// ***
template<scan::HttpBody T>
inline void scan::Response<T>::parse(const response_t &t_resp)
{
    m_resp = t_resp;
    m_status = static_cast<status_t>(t_resp.result());
    m_valid = true;

    this->m_body = t_resp.body();

    update_msg();
}

/// ***
/// Parse information from the given raw HTTP response
/// ***
template<scan::HttpBody T>
inline void scan::Response<T>::parse(const string &t_raw_resp)
{
    if (t_raw_resp.empty())
    {
        throw ArgEx{ "t_raw_resp", "Raw response cannot be empty" };
    }

    size_t offset{ 0 };
    http::response_parser<T> parser;

    // Add buffer data until fully processed
    while (!parser.is_done() && m_valid)
    {
        error_code ecode;

        const string resp_data{ t_raw_resp.substr(offset) };
        const asio::const_buffer resp_buffer{ asio::buffer(resp_data) };

        offset += parser.put(resp_buffer, ecode);
        m_valid = NetUtil::no_error(ecode);
    }
    parse(parser.get());
}

/// ***
/// Synchronize the underlying response header fields and member header fields
/// ***
template<scan::HttpBody T>
inline void scan::Response<T>::update_fields()
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

    // Update response using member headers
    for (const field_kv &field_kvp : this->m_fields)
    {
        m_resp.set(field_kvp.first, field_kvp.second);
    }
    this->add_fields(m_resp.base());
}

/// ***
/// Update the underlying HTTP response using the current member values
/// ***
template<scan::HttpBody T>
inline void scan::Response<T>::update_msg()
{
    update_fields();

    m_resp.body() = this->m_body;
    m_resp.prepare_payload();
    m_resp.result(static_cast<uint>(m_status));

    update_fields();
}

/// ***
/// Determine whether the underlying HTTP response is valid
/// ***
template<scan::HttpBody T>
inline bool scan::Response<T>::valid() const
{
    return m_valid;
}

/// ***
/// Determine whether the underlying response contains HTTP status code 200 (OK)
/// ***
template<scan::HttpBody T>
inline bool scan::Response<T>::ok() const noexcept
{
    return m_status == status_t::ok;
}

/// ***
/// Get the HTTP response status code as an enumeration type
/// ***
template<scan::HttpBody T>
inline http::status scan::Response<T>::status() const noexcept
{
    return m_status;
}

/// ***
/// Get the HTTP response status code as an unsigned integer
/// ***
template<scan::HttpBody T>
inline unsigned int scan::Response<T>::status_code() const noexcept
{
    return static_cast<uint>(m_status);
}

/// ***
/// Set the string body value in the underlying HTTP message
/// ***
template<scan::HttpBody T>
inline std::string scan::Response<T>::body(const string &t_body,
                                           const string &t_mime) {
    this->m_body = t_body;
    this->content_type = t_mime;

    update_msg();

    return this->m_body;
}

/// ***
/// Get the underlying HTTP message header string
/// ***
template<scan::HttpBody T>
inline std::string scan::Response<T>::msg_header()
{
    std::stringstream ss;
    ss << m_resp.base();

    return ss.str();
}

/// ***
/// Get the 'Server' header field value from the underlying response
/// ***
template<scan::HttpBody T>
inline std::string scan::Response<T>::server() const
{
    string field_val;

    if (this->contains_field("Server"))
    {
        field_val = this->m_fields.at("Server");
    }
    return field_val;
}

/// ***
/// Get the start-line from the underlying HTTP response header
/// ***
template<scan::HttpBody T>
inline std::string scan::Response<T>::start_line() const
{
    return Util::fstr("% % %",
                      static_cast<string>(this->httpv),
                      static_cast<uint>(m_status),
                      status_str());
}

/// ***
/// Get the HTTP response status as a string
/// ***
template<scan::HttpBody T>
inline std::string scan::Response<T>::status_str() const
{
    std::stringstream ss;
    ss << m_status;

    return ss.str();
}

/// ***
/// Get a reference to the underlying HTTP response
/// ***
template<scan::HttpBody T>
inline const http::response<T> &scan::Response<T>::response() const noexcept
{
    return m_resp;
}

/// ***
/// Get a reference to the underlying HTTP response
/// ***
template<scan::HttpBody T>
inline http::response<T> &scan::Response<T>::response() noexcept
{
    return m_resp;
}

/// ***
/// Validate the HTTP header entries in the underlying header field map
/// ***
template<scan::HttpBody T>
inline void scan::Response<T>::validate_fields() const
{
    const string caller{ "Response<T>::validate_fields" };
    field_map::const_iterator server_it{ this->m_fields.find("Server") };

    // Missing 'Server' header key
    if (server_it == this->m_fields.end())
    {
        throw RuntimeEx{ caller, "Missing required header 'Server'" };
    }

    // Missing 'Server' header value
    if (server_it->second.empty())
    {
        throw RuntimeEx{ caller, "Missing value for required header 'Server'" };
    }
}

/// ***
/// Get the underlying response as a raw string
/// ***
template<scan::HttpBody T>
inline std::string scan::Response<T>::raw()
{
    update_msg();

    std::stringstream ss;
    ss << m_resp;

    return ss.str();
}

#endif // !RESPONSE_H
