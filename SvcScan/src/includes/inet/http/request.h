/*
*  request.h
*  ---------
*  Header file for a HTTP network request message
*/
#pragma once

#ifndef REQUEST_H
#define REQUEST_H

#include "httpmsg.h"

namespace scan
{
    /// ***
    /// HTTP network request class
    /// ***
    class Request final : public HttpMsg
    {
    private:  /* Types */
        using base = HttpMsg;

    public:  /* Constants */
        static constexpr char GET[]  = "GET";   // HTTP GET method
        static constexpr char HEAD[] = "HEAD";  // HTTP HEAD method
        static constexpr char POST[] = "POST";  // HTTP POST method

    public:  /* Fields */
        AutoProp<string> accept;      // 'Accept' header
        AutoProp<string> host;        // 'Host' header
        AutoProp<string> referer;     // 'Referer' header
        AutoProp<string> uri;         // HTTP request URI
        AutoProp<string> user_agent;  // 'User-Agent' header

    private:  /* Fields */
        string m_method;  // HTTP request method

    public:  /* Constructors & Destructor */
        Request(const Request &t_request);

        Request(const string &t_method,
                const string &t_host,
                const string &t_payload = string(),
                const string &t_uri = "/");

        virtual ~Request() = default;

    private:  /* Constructors */
        Request() = delete;

    public:  /* Operators */
        Request &operator=(const Request &t_request);

        operator string() const override;

        friend std::ostream &operator<<(std::ostream &t_os,
                                        const Request &t_request);

    public:  /* Methods */
        string payload(const string &t_payload,
                       const string &t_mime = string()) override;

        header_map update_headers() override;

    private:  /* Methods */
        void validate_headers(const header_map &t_headers) const override;

        string raw() override;
    };
}

#endif // !REQUEST_H
