/*
* @file
*     http_version.h
* @brief
*     Header file for an HTTP protocol version.
*/
#pragma once

#ifndef SCAN_HTTP_VERSION_H
#define SCAN_HTTP_VERSION_H

#include <string>
#include "../../contracts/i_string_castable.h"
#include "../../utils/algo.h"
#include "../../utils/aliases.h"
#include "../net_const_defs.h"

namespace scan
{
    /**
    * @brief
    *     HTTP protocol version.
    */
    class HttpVersion : public IStringCastable
    {
    private:  /* Constants */
        static constexpr cstr_t DELIM = "/";  // Version prefix delimiter

    public:  /* Fields */
        uint_t major;  // Major version number
        uint_t minor;  // Minor version number

    public:  /* Constructors & Destructor */
        /**
        * @brief
        *     Initialize the object.
        */
        constexpr HttpVersion() noexcept : HttpVersion{ 1U, 1U }
        {
        }

        constexpr HttpVersion(const HttpVersion&) = default;
        constexpr HttpVersion(HttpVersion&&) = default;

        /**
        * @brief
        *     Initialize the object.
        */
        constexpr HttpVersion(const uint_t& t_major, const uint_t& t_minor) noexcept
        {
            major = t_major;
            minor = t_minor;
        }

        HttpVersion(const string& t_httpv_str);

        virtual constexpr ~HttpVersion() = default;

    public:  /* Operators */
        constexpr HttpVersion& operator=(const HttpVersion&) = default;
        constexpr HttpVersion& operator=(HttpVersion&&) = default;

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator uint_t() const noexcept
        {
            return num();
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator int() const noexcept
        {
            return static_cast<int>(num());
        }

        /**
        * @brief
        *     Cast operator overload.
        */
        constexpr operator string() const override
        {
            return str();
        }

        friend ostream& operator<<(ostream& t_os, const HttpVersion& t_httpv);

    public:  /* Methods */
        /**
        * @brief
        *     Get the calculated current version number.
        */
        constexpr uint_t num() const noexcept
        {
            return (major * 10U) + minor;
        }

        /**
        * @brief
        *     Get the current version number as a string in decimal format.
        */
        constexpr string num_str() const
        {
            return algo::fstr("%.%", major, minor);
        }

        /**
        * @brief
        *     Get the current version number as a string that can be used
        *     in the start-line of an HTTP request (e.g., HTTP/1.1).
        */
        constexpr string str() const
        {
            return algo::concat(PREFIX, DELIM, num_str());
        }
    };

    /**
    * @brief
    *     Bitwise left shift operator overload.
    */
    inline ostream& operator<<(ostream& t_os, const HttpVersion& t_httpv)
    {
        return t_os << t_httpv.str();
    }
}

#endif // !SCAN_HTTP_VERSION_H
