/*
* @file
*     http_version.h
* @brief
*     Header file for an HTTP protocol version.
*/
#pragma once

#ifndef HTTP_VERSION_H
#define HTTP_VERSION_H

#include "../../contracts/i_string_castable.h"
#include "../../utils/algorithm.h"

namespace scan
{
    /**
    * @brief
    *     HTTP protocol version.
    */
    class HttpVersion : public IStringCastable
    {
    private:  /* Type Aliases */
        using this_t = HttpVersion;

        using algo = Algorithm;

    private:  /* Constants */
        static constexpr cstr_t DELIM  = "/";  // Version prefix delimiter

    public:  /* Fields */
        uint_t major;  // Major version number
        uint_t minor;  // Minor version number

    public:  /* Constructors & Destructor */
        HttpVersion() noexcept;
        HttpVersion(const HttpVersion &t_httpv) noexcept;
        HttpVersion(HttpVersion &&) = default;
        HttpVersion(const uint_t &t_major, const uint_t &t_minor) noexcept;
        HttpVersion(const string &t_httpv_str);

        virtual ~HttpVersion() = default;

    public:  /* Operators */
        HttpVersion &operator=(const HttpVersion &t_httpv) noexcept;
        HttpVersion &operator=(HttpVersion &&) = default;

        operator scan::uint_t() const noexcept;
        operator int() const noexcept;

        operator std::string() const override;

        friend ostream &operator<<(ostream &t_os, const HttpVersion &t_httpv);

    public:  /* Methods */
        uint_t num() const noexcept;

        string num_str() const;
        string str() const;
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream &operator<<(ostream &t_os, const HttpVersion &t_httpv)
    {
        return t_os << t_httpv.str();
    }
}

#endif // !HTTP_VERSION_H
