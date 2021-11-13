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
        static constexpr char CRLF[] = "\r\n";

    public:  /* Fields */
        AutoProp<uint> code;      // HTTP status code
        AutoProp<string> date;    // 'Date' header
        AutoProp<string> server;  // 'Server' header
        AutoProp<string> status;  // HTTP status name

    public:  /* Constructors & Destructor */
        Response(const Response &t_response);
        Response(const string &t_raw_resp);

        virtual ~Response() = default;

    private:  /* Constructors */
        Response() = delete;

    public:  /* Operators */
        Response &operator=(const Response &t_response);
        Response &operator=(const string &t_raw_resp);

        operator string() const override;

        friend std::ostream &operator<<(std::ostream &t_os,
                                        const Response &t_response);

    public:  /* Methods */
        string raw() const override;

    private:  /* Methods */
        void update_members();

        string parse_payload(const string &t_raw_payload,
                             const size_t &t_content_len);

        Response &parse(const string &t_raw_resp);

        header_map parse_headers(const string &t_raw_headers);
        header_map update_headers() override;
    };
}

#endif // !RESPONSE_H
