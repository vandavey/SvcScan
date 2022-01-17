/*
*  response.h
*  ----------
*  Header file for a HTTP network response message
*/
#pragma once

#ifndef RESPONSE_H
#define RESPONSE_H

#include "httpmsg.h"

namespace scan
{
    /// ***
    /// HTTP network request class
    /// ***
    class Response final : public HttpMsg
    {
    private:  /* Types */
        using base = HttpMsg;

        using uint = unsigned int;

    private:  /* Constants */
        static constexpr char CRLF[]         = "\r\n";   // Carriage-return
        static constexpr char HTTPV_PREFIX[] = "HTTP";  // HTTP version prefix

    public:  /* Fields */
        uint code;      // HTTP status code

        string date;    // 'Date' header
        string server;  // 'Server' header
        string status;  // HTTP status name

    private:  /* Fields */
        bool m_is_valid;  // Valid HTTP response

    public:  /* Constructors & Destructor */
        Response();
        Response(const Response &t_response);
        Response(const string &t_raw_resp);

        virtual ~Response() = default;

    public:  /* Operators */
        Response &operator=(const Response &t_response);
        Response &operator=(const string &t_raw_resp);

        operator string() const override;

        friend std::ostream &operator<<(std::ostream &t_os,
                                        const Response &t_response);

    public:  /* Methods */
        bool valid(const bool &t_check_server = true) const;

        string get_server() const;

        Response &parse(const string &t_raw_resp);

    private:  /* Methods */
        void update_members();

        string parse_payload(const string &t_raw_payload,
                             const size_t &t_content_len);

        string raw() override;

        header_map parse_headers(const string &t_raw_headers);
        header_map update_headers() override;
    };
}

#endif // !RESPONSE_H
