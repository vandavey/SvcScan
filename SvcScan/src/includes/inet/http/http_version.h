/*
*  http_version.h
*  --------------
*  Header file for an HTTP protocol version
*/
#pragma once

#ifndef HTTP_VERSION_H
#define HTTP_VERSION_H

#include <iostream>
#include <vector>
#include "../../contracts/i_string_castable.h"

namespace scan
{
    /**
    * @brief  HTTP protocol version.
    */
    class HttpVersion : public IStringCastable
    {
    private:  /* Type Aliases */
        using this_t = HttpVersion;

        using uint = unsigned int;

        using string = std::string;

        template<class T>
        using vector = std::vector<T>;

    private:  /* Constants */
        static constexpr char DELIM[]  = "/";     // Prefix delimiter
        static constexpr char PREFIX[] = "HTTP";  // Version prefix

    public:  /* Fields */
        uint major;  // Major version number
        uint minor;  // Minor version number

    public:  /* Constructors & Destructor */
        HttpVersion() noexcept;
        HttpVersion(const HttpVersion &t_httpv);
        HttpVersion(HttpVersion &&) = default;
        HttpVersion(const uint &t_major, const uint &t_minor);
        HttpVersion(const string &t_version_str);

        virtual ~HttpVersion() = default;

    public:  /* Operators */
        HttpVersion &operator=(const HttpVersion &t_httpv) noexcept;
        HttpVersion &operator=(HttpVersion &&) = default;

        operator uint() const noexcept;
        operator int() const noexcept;

        operator string() const override;

        friend std::ostream &operator<<(std::ostream &t_os,
                                        const HttpVersion &t_httpv);

    public:  /* Methods */
        uint num() const noexcept;

        string num_str() const;
        string str() const;
    };

    /**
    * @brief  Bitwise left shift operator overload.
    */
    inline std::ostream &operator<<(std::ostream &t_os, const HttpVersion &t_httpv)
    {
        return t_os << t_httpv.str();
    }
}

#endif // !HTTP_VERSION_H
